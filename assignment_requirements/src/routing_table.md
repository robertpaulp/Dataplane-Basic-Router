# Tabela de rutare

Fiecare router dispune de o "tabelă de rutare" -- o structură pe baza
căreia alege portul pe care să emită un pachet. În mod normal, aceste
tabele sunt populate de câtre control plane, în urma rulării unui
algoritm de rutare (e.g. OSPF); în cadrul temei, vom folosi o tabelă
statică.

O intrare în tabel are patru coloane:

-   `prefix` și `mask`: împreună aceste două câmpuri formează o adresă
    de rețea (e.g. `192.168.1.0/24`)

-   `next hop`: adresa IP a mașinii către care va fi trimis pachetul;
    dacă routerul nu e conectat direct la destinatar, aceasta va fi
    adresa unui router intermediar.

-   `interface`: identificatorul interfeței pe care routerul va trebui
    să trimită pachetul respectiv către `next hop`.

``` {#list:first label="list:first" caption="Exemplu de Tabelă de rutare"}
Prefix         Next hop        Mask             Interface
192.168.0.0    192.168.0.2     255.255.255.0    0
192.168.1.0    192.168.1.2     255.255.255.0    1
192.168.2.0    192.168.2.2     255.255.255.0    2
192.168.3.0    192.168.3.2     255.255.255.0    3
```

În contextul temei, este suficient să considerați că numărul maxim de
intrări din tabela de rutare este de 100 000 de intrări.

