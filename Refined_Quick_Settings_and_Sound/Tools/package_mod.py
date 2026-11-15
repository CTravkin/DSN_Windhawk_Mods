from pathlib import Path
import re
root=Path(__file__).resolve().parent.parent/'Mod'
seen=set()
def expand(path):
    if path.name in seen:return ''
    seen.add(path.name)
    text=path.read_text(encoding='utf-8-sig')
    text=re.sub(r'^#pragma once\s*$', '',text,flags=re.M)
    return re.sub(r'^#include "([^"]+)"\s*$',lambda m:expand(path.parent/m[1]),text,flags=re.M)
source=expand(root/'Source'/'sound-native.cpp')
mod_id=re.search(r'^// @id\s+([a-z0-9-]+)$',source,flags=re.M)[1]
# One copy of each system include in the single file (the first one stays).
included=set()
def once(m):
    if m[1] in included:return ''
    included.add(m[1]);return m[0]
source=re.sub(r'^#include (<[^>]+>)[^\n]*\n',once,source,flags=re.M)
for forbidden in ['CreateWindowExW(', 'DesktopWindowXamlSource', 'WindowsXamlManager::Initialize']:
    assert forbidden not in source, forbidden
target=root/(mod_id+'.wh.cpp')
target.write_text(source+'\n/* Lucide license:\n'+(root/'LUCIDE-LICENSE.txt').read_text(encoding='utf-8')+'\n*/\n',encoding='utf-8')
print('Native source packaged:',target)
