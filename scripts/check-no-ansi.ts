import { readdir, readFile } from "node:fs/promises";
import { join } from "node:path";

const ALLOWED_FILES = new Set([join("packages", "term", "src", "sgr.ts")]);

const ANSI_REGEX = /(?:\x1b|\\x1b|\\u001b|\\033|\\e\[)/;

async function scanDir(dir: string): Promise<string[]> {
  const violations: string[] = [];
  const entries = await readdir(dir, { withFileTypes: true });

  for (const entry of entries) {
    const fullPath = join(dir, entry.name);
    if (entry.isDirectory()) {
      if (entry.name === "node_modules" || entry.name === "dist" || entry.name === ".git") {
        continue;
      }
      violations.push(...(await scanDir(fullPath)));
    } else if (entry.isFile() && (entry.name.endsWith(".ts") || entry.name.endsWith(".js"))) {
      if (
        ALLOWED_FILES.has(fullPath) ||
        fullPath.includes(".test.") ||
        fullPath.includes("__tests__")
      ) {
        continue;
      }
      const content = await readFile(fullPath, "utf8");
      if (ANSI_REGEX.test(content)) {
        violations.push(fullPath);
      }
    }
  }
  return violations;
}

const violations = await scanDir("packages");
if (violations.length > 0) {
  console.error(
    "Guardrail violation: ANSI escape sequences detected outside packages/term/src/sgr.ts:",
  );
  for (const v of violations) {
    console.error(`  - ${v}`);
  }
  process.exit(1);
}
console.log("check-no-ansi: OK");
