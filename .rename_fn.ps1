$ErrorActionPreference = 'Stop'
$root = 'd:/Code/fsm'
$Apply = $args[0] -eq '-apply'

$files = @(Get-ChildItem -Path (Join-Path $root '*.h') -File) + @(Get-ChildItem -Path (Join-Path $root '*.cpp') -File)
$all = @{}
foreach ($f in $files) { $all[$f.FullName] = [System.IO.File]::ReadAllText($f.FullName) }
$fullText = ($all.Values -join "`n")

$classNames = @([regex]::Matches($fullText, '\b(?:class|struct)\s+([A-Za-z_][A-Za-z0-9_]*)') | ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique)
$keywords = @('return','else','case','new','delete','throw','sizeof','if','while','for','switch','catch','typedef','using','decltype','alignas','static_assert','reinterpret_cast','static_cast','dynamic_cast','const_cast','do','goto')
$exclude = @('WinMain','WindowProc','DebugWindowProc','main','wWinMain','DllMain')
# local-variable names misidentified as functions (iterator locals in templates)
$exclude += @('NodeItr','EdgeItr','ConstEdgeItr')

$defPattern = '(?m)^[ \t]*(?:virtual[ \t]+|static[ \t]+|inline[ \t]+|explicit[ \t]+|constexpr[ \t]+|friend[ \t]+)*([A-Za-z_][\w\s:<>,\*&]*?)[ \t]+([A-Z][A-Za-z0-9_]*)[ \t]*\('

$names = [ordered]@{}
foreach ($f in $files) {
  foreach ($m in [regex]::Matches($all[$f.FullName], $defPattern)) {
    $type = $m.Groups[1].Value.Trim()
    $nm   = $m.Groups[2].Value
    if ($keywords -contains $type) { continue }
    if ($classNames -contains $nm) { continue }
    if ($exclude -contains $nm)    { continue }
    $names[$nm] = $true
  }
}
$sorted = @($names.Keys | Sort-Object)

function To-Camel([string]$n) {
  if ($n -cmatch '^[A-Z][A-Z0-9_]*$') { return $n.ToLower() }  # BFS/DFS/ID -> bfs/dfs/id
  return $n.Substring(0,1).ToLower() + $n.Substring(1)
}

$map = [ordered]@{}
$collisions = @()
foreach ($n in $sorted) {
  $new = To-Camel $n
  if ($new -ceq $n) { continue }
  $map[$n] = $new
  if ([regex]::IsMatch($fullText, '\b' + [regex]::Escape($new) + '\s*\(')) {
    $collisions += ("$n -> $new")
  }
}

$report = @()
$report += "===== 映射 (共 $($map.Count) 个) ====="
foreach ($k in $map.Keys) { $report += ("  {0,-38} -> {1}" -f $k, $map[$k]) }
$report += ""
$report += "===== 冲突/跳过 (新名后跟 '(' 已存在，跳过以免重名) 共 $($collisions.Count) 个 ====="
$collisions | ForEach-Object { $report += ("  " + $_) }
[System.IO.File]::WriteAllLines((Join-Path $root '.rename_report.txt'), $report, [System.Text.Encoding]::UTF8)

Write-Host ("映射 {0} 个, 冲突 {1} 个, 报告已写入 .rename_report.txt" -f $map.Count, $collisions.Count)

if ($Apply) {
  foreach ($f in $files) {
    $t = $all[$f.FullName]
    foreach ($n in $sorted) {
      if (-not $map.Contains($n)) { continue }
      if ($collisions -contains ($n + " -> " + $map[$n])) { continue }
      # paren-aware: only rename when followed by optional whitespace then (
      $t = [regex]::Replace($t, '\b' + [regex]::Escape($n) + '(?=\s*\()', $map[$n])
    }
    if ($t -ne $all[$f.FullName]) {
      [System.IO.File]::WriteAllText($f.FullName, $t)
      Write-Host ("  updated {0}" -f $f.Name)
    }
  }
}
