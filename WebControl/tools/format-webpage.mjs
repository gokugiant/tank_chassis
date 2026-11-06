// Prettier versteht keine C++-Header. Deshalb nur den HTML-Raw-String formatieren.
import { readFile, writeFile } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import * as prettier from 'prettier';

const file = fileURLToPath(new URL('../include/WebPage.h', import.meta.url));
const content = await readFile(file, 'utf8');
const start = content.indexOf('R"HTML(') + 'R"HTML('.length;
const end = content.lastIndexOf(')HTML";');
if (start < 'R"HTML('.length || end < start) {
  throw new Error('HTML-Raw-String in WebPage.h nicht gefunden.');
}
const config = await prettier.resolveConfig(file);
const html = await prettier.format(content.slice(start, end).trim(), {
  ...config,
  parser: 'html',
});
await writeFile(file, content.slice(0, start) + '\n' + html + content.slice(end));
console.log('include/WebPage.h: HTML, CSS und JavaScript formatiert');
