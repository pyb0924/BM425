$files= Get-ChildItem -Path .\*.dcm 
foreach($file in $files)
{
    storescu.exe -d -aec ARCHIVE localhost 104 $file
}