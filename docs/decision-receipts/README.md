# Decision Receipts

Use a receipt for a material Pocket OS architecture, product, or hardware decision whose outcome should be checked after build or device evidence exists.

1. Copy `TEMPLATE.yaml` to `<YYYY-MM-DD>-<decision-id>.yaml`.
2. Record the owner, stakes, reversibility, rationale, expected outcome, revisit date, and observable criterion.
3. Link source, build, and device evidence separately.
4. At revisit, record `CONFIRMED`, `REVISED`, `REVERSED`, or `INCONCLUSIVE`.
5. Preserve the original. Revisions and reversals link a replacement receipt.

Receipts do not authorize build, flash, device writes, merge, release, or external actions. Hardware claims require device evidence; build-only evidence leaves them `INCONCLUSIVE`.
