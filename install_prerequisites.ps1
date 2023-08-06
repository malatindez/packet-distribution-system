
$url = 'https://deac-riga.dl.sourceforge.net/project/boost/boost/1.81.0/boost_1_81_0.zip'
$output = Resolve-Path '.\third_party\'
$output = $output.Path + 'boost_1_81_0.zip'

if (!(Test-Path -Path $output)) {
    $webClient = New-Object System.Net.WebClient

    $event1 = Register-ObjectEvent $webClient DownloadProgressChanged -Action {
        Write-Progress -Id 1 -Activity "Downloading file..." -Status "$($EventArgs.ProgressPercentage)% Complete:" -PercentComplete $EventArgs.ProgressPercentage
    }

    $event2 = Register-ObjectEvent $webClient DownloadFileCompleted -Action {
        Write-Progress -Id 1 -Activity "Downloading file..." -Completed
        Write-Host "Download Complete!"
    }

    # Start the download
    $webClient.DownloadFileAsync($url, $output)

    # Wait until download completes
    while ($webClient.IsBusy) {
        Start-Sleep -Milliseconds 100
    }

    Write-Progress -Id 1 -Activity "Downloading file..." -Completed
    # Cleanup WebClient after download
    $webClient.Dispose()

    Unregister-Event -SourceIdentifier $event1.Name
    Unregister-Event -SourceIdentifier $event2.Name

    # Remove the event jobs
    Remove-Job -Name $event1.Name
    Remove-Job -Name $event2.Name
}
else {
    Write-Host "File already exists. Skipping download..."
}
# Add .NET assembly
Add-Type -AssemblyName System.IO.Compression.FileSystem

function Unzip
{
    param([string]$zipfile, [string]$outpath)

    if (Test-Path -Path $zipfile) {
        if (!(Test-Path -Path $outpath)) {
            New-Item -ItemType Directory -Force -Path $outpath | Out-Null
        }

        $archive = [System.IO.Compression.ZipFile]::OpenRead($zipfile)
        $totalFiles = $archive.Entries.Count
        $fileCounter = 0

        foreach ($entry in $archive.Entries) {
            $destinationPath = [System.IO.Path]::Combine($outpath, $entry.FullName)

            # Create parent directory regardless of whether it's a file or a directory
            $parent = [System.IO.Directory]::GetParent($destinationPath)
            if (!(Test-Path $parent)) {
                New-Item -ItemType Directory -Force -Path $parent.FullName | Out-Null
            }

            if (!$entry.FullName.EndsWith("/")) { # If it's a file, extract it
                $fileCounter++
                Write-Progress -Id 1 -Activity "Extracting Files" -Status "$fileCounter of $totalFiles" -PercentComplete (($fileCounter / $totalFiles) * 100)
                [System.IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $destinationPath, $true)
            }
        }

        $archive.Dispose()
    } else {
        Write-Output "The zip file $zipfile does not exist"
    }
}


# Call the function
Unzip $output 'third_party'
Remove-Item $zipfile
$oldFolder = Get-Item '.\third_party\boost_*' # Adjust the path as necessary
Rename-Item -Path $oldFolder[0].FullName -NewName 'boost'