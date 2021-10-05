# $files= Get-ChildItem -Path .\data\*.dcm -Recurse
$files= Get-ChildItem -Path .\*.dcm 
foreach($file in $files)
{
    storescu.exe -d -aec ARCHIVE localhost 104 $file
}
    
