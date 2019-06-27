lcc -I%IDF_PATH%\components\soc\esp32\include -I%IDF_PATH%\components\soc\esp32\include\soc -I%ULPCCDIR%\inc -Wf-target=ulp -S %1 -o %~n1.S

