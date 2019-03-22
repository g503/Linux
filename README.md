Przykładowy projekt do utworzenia 'char' modułu pod linuxem
Realzuje zapis/odczyt danych znakowych

Jak uruchomić:
==============

1. Kompilacja
make

2. Instalacja modułu
sudo insmod ./hello.ko

3. Poszukaj numer mudułu o nazwie helloDevName
sudo cat /proc/devices | grep hello
lub uruchom w osobnym oknie logera systemowego i odczytaj numer z konsoli
sudo tail -f /var/log/syslog

4. Wygenerowanie specjalnego pliku do komunikacji z userem
sudo mknod hello c 243 0
#243 to numer modułu odczytanego z punktu 3

5. Czytanie i zapis
Wersja 1.
sudo chmod 666 /dev/helloDevName
cat /dev/helloDevName
sudo echo "Tu wpisujemy tekst do wyslania" > /dev/helloDevName
Wersja 2.
Zapis/odczyt realizowany jest poprzez aplikacje test
sudo ./test

Jak odinstalować i posprzątać
=============================
1. Odłącz moduł
sudo rmmod hello

2. Usuń produkty kompilacji
sudo rm hello
make clean

