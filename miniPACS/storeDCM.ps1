$files= Get-ChildItem -Path .\*.dcm 
foreach($file in $files)
{
    storescu.exe -d -aec SERVER localhost 104 $file
}