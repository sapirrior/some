import { readdir, readFile } from "node:fs/promises";
import { join } from "node:path";

// Hex colors (#fff or #ffffff) or rgb( / rgba(
const COLOR_REGEX = /(?:#[0-9a-fA-F]{3,8}\b|rgba?\s*\()/;

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
      if (fullPath.includes(".test.") || fullPath.includes("__tests__")) {
        continue;
      }
      const content = await readFile(fullPath, "utf8");
      if (COLOR_REGEX.test(content)) {
        violations.push(fullPath);
      }
    }
  }
  return violations;
}

const violations = await scanDir("packages");
if (violations.length > 0) {
  console.error(
    "Guardrail violation: Hardcoded colors detected in packages/ (colors must live in themes/*.json only):",
  );
  for (const v of violations) {
    console.error(`  - ${v}`);
  }
  process.exit(1);
}
console.log("check-no-colors: OK");
