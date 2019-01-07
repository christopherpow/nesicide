if "%1"=="install" goto :install
goto :done

:install 
  choco install qtcreator

:done
  exit 0

