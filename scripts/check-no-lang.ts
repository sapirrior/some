import { readdir, readFile } from "node:fs/promises";
import { join } from "node:path";

// Flag common file extension mappings or keyword list arrays in packages/
const EXTENSION_MAP_REGEX = /\b(?:extToLang|languageExtensions|extensionMap)\b/;

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
      if (EXTENSION_MAP_REGEX.test(content)) {
        violations.push(fullPath);
      }
    }
  }
  return violations;
}

const violations = await scanDir("packages");
if (violations.length > 0) {
  console.error(
    "Guardrail violation: Language extension maps or hardcoded language knowledge detected in packages/ (must live in grammars/** only):",
  );
  for (const v of violations) {
    console.error(`  - ${v}`);
  }
  process.exit(1);
}
console.log("check-no-lang: OK");
