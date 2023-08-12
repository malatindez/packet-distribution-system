# Example using a for loop
$numberOfProcesses = 250

for ($i = 1; $i -le $numberOfProcesses; $i++) {
    Start-Process -FilePath "client_test.exe"
}
