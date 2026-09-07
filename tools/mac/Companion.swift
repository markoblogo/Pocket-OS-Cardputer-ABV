import Cocoa
import WebKit

final class Companion: NSObject, NSApplicationDelegate, WKNavigationDelegate {
    var window: NSWindow!
    var web: WKWebView!
    var server: Process?
    var log: FileHandle?
    var attempts = 0
    let port = 18765
    func applicationDidFinishLaunching(_ notification: Notification) {
        NSApp.setActivationPolicy(.regular)
        let menu = NSMenu(); let item = NSMenuItem(); menu.addItem(item)
        let submenu = NSMenu(); submenu.addItem(withTitle: "Quit Cardputer Companion", action: #selector(NSApplication.terminate(_:)), keyEquivalent: "q"); item.submenu = submenu; NSApp.mainMenu = menu
        window = NSWindow(contentRect: NSRect(x: 0, y: 0, width: 1000, height: 760), styleMask: [.titled,.closable,.miniaturizable,.resizable], backing: .buffered, defer: false)
        window.title = "Cardputer Companion"; window.minSize = NSSize(width: 540, height: 550)
        web = WKWebView(); web.navigationDelegate = self; window.contentView = web
        window.center(); window.makeKeyAndOrderFront(nil); NSApp.activate(ignoringOtherApps: true)
        web.loadHTMLString("<html><body style='font:18px -apple-system;padding:50px;background:#f5f5f0'><h2>Cardputer Companion</h2><p>Запускаем приложение…</p></body></html>", baseURL:nil)
        start()
    }
    func start() {
        guard let resources = Bundle.main.resourceURL,
              let python = Bundle.main.object(forInfoDictionaryKey:"ABVxPython") as? String else { fail("Не найден Python. Повторите установку Companion."); return }
        let fm = FileManager.default
        let support = fm.homeDirectoryForCurrentUser.appendingPathComponent("Library/Application Support/ABVx Companion")
        do {
            try fm.createDirectory(at:support,withIntermediateDirectories:true)
            let logfile = support.appendingPathComponent("desktop.log")
            if !fm.fileExists(atPath:logfile.path) { fm.createFile(atPath:logfile.path,contents:nil) }
            log = try FileHandle(forWritingTo:logfile);log?.seekToEndOfFile()
            let process = Process();process.executableURL = URL(fileURLWithPath:python)
            process.arguments = [resources.appendingPathComponent("abvx_companion_app.py").path,"--no-open","--port",String(port)]
            var env = ProcessInfo.processInfo.environment
            env["PATH"] = "/opt/homebrew/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin"
            if let project = Bundle.main.object(forInfoDictionaryKey:"ABVxProjectRoot") as? String { env["ABVX_PROJECT_ROOT"] = project }
            env["PYTHONUNBUFFERED"] = "1"
            process.environment=env;process.currentDirectoryURL=resources
            process.standardOutput=log;process.standardError=log
            server=process;try process.run();waitForServer()
        } catch { fail("Не удалось запустить сервис: \(error.localizedDescription)") }
    }
    func waitForServer() {
        guard server?.isRunning == true else {fail("Сервис не запустился. Возможно, порт 18765 занят. Подробности: Library/Application Support/ABVx Companion/desktop.log");return}
        var request=URLRequest(url:URL(string:"http://127.0.0.1:\(port)/api/status")!);request.timeoutInterval=2
        URLSession.shared.dataTask(with:request){data,response,error in DispatchQueue.main.async {
            if let response=response as? HTTPURLResponse,response.statusCode==200, let data=data,
               let status=(try? JSONSerialization.jsonObject(with:data)) as? [String:Any], status["usb_ports"] != nil {
                self.web.load(URLRequest(url:URL(string:"http://127.0.0.1:\(self.port)/")!))
            } else {self.attempts+=1;if self.attempts<20{DispatchQueue.main.asyncAfter(deadline:.now()+0.5){self.waitForServer()}}else{self.fail("Сервис не отвечает. Закройте приложение и попробуйте снова.")}}
        }}.resume()
    }
    func fail(_ message:String) {
        let alert=NSAlert();alert.messageText="Companion не запущен";alert.informativeText=message;alert.runModal()
    }
    func applicationShouldTerminateAfterLastWindowClosed(_ sender:NSApplication)->Bool {true}
    func applicationShouldTerminate(_ sender:NSApplication)->NSApplication.TerminateReply {
        guard server?.isRunning == true else{return .terminateNow}
        var request=URLRequest(url:URL(string:"http://127.0.0.1:\(port)/api/status")!);request.timeoutInterval=3
        URLSession.shared.dataTask(with:request){data,_,_ in DispatchQueue.main.async {
            guard let data=data,let result=(try? JSONSerialization.jsonObject(with:data)) as? [String:Any],let job=result["job"] as? [String:Any],let state=job["state"] as? String else {
                let a=NSAlert();a.messageText="Не удалось узнать состояние операции";a.informativeText="Приложение останется открытым, чтобы не прервать запись файлов.";a.runModal();sender.reply(toApplicationShouldTerminate:false);return
            }
            if state.uppercased()=="RUNNING" {let a=NSAlert();a.messageText="Дождитесь завершения операции";a.informativeText="Закрытие сейчас может прервать перенос файлов.";a.runModal();sender.reply(toApplicationShouldTerminate:false)}
            else{self.server?.terminate();sender.reply(toApplicationShouldTerminate:true)}
        }}.resume();return .terminateLater
    }
    func webView(_ webView:WKWebView,decidePolicyFor navigationAction:WKNavigationAction,decisionHandler:@escaping(WKNavigationActionPolicy)->Void){
        guard let url=navigationAction.request.url else{decisionHandler(.cancel);return}
        if url.scheme=="about" || (url.scheme=="http" && url.host=="127.0.0.1" && url.port==port){decisionHandler(.allow)}else{decisionHandler(.cancel)}
    }
}
let app=NSApplication.shared
let delegate=Companion();app.delegate=delegate;app.run()
