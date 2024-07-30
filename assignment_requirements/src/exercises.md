# Cerințe temă

> În [acest repo](https://gitlab.cs.pub.ro/pcom/homework1-public) găsiți scheletul temei, infrastructura și checker-ul automat.

Pentru rezolvarea temei, trebuie să implementați dataplane-ul unui
router.
**Va recomandăm să folosiți cel puțin `ping` pentru a testa
implementarea și `Wireshark` sau `tcpdump` pentru depanare și analiza corectitudinii.**
Punctajul este împărțit în mai multe componente, după cum urmează:

-   **Procesul de dirijare (30p).** Va trebui să implementați pașii
    prezentați în [secțiunea IPv4](ip.html). Pentru acest exercițiu nu este
    nevoie să implementați și funcționalitatea referitoare la ICMP și puteți
    folosi o tabela statică de ARP.
    
-   **Longest Prefix Match eficient (16p).** La laborator, am
    implementat LPM folosind o căutare liniară, i.e. trecând de fiecare
    dată prin toate întrările din tabel. În practică, o tabelă de
    routare poate conține foarte multe întrări [^1] -- astfel de căutare
    este ineficientă. O abordare mai bună este să folosim o
    [trie](https://www.lewuathe.com/longest-prefix-match-with-trie-tree.html).

    Orice implementare mai eficientă decât căutarea liniară valorează 16
    puncte.

-   **Protocolul ARP (33p).** Vom implementa pașii din [secțiunea ARP](arp.md) pentru
    a popula dinamic tabela ARP.      Va trebui să
    implementați și funcționalitatea de caching; după ce primiți un răspuns ARP,
    rețineți adresa MAC a hostului interogat. În realitate, intrările dintr-un
    cache sunt temporare, fiind șterse după un anumit interval de timp. Pentru
    implementarea temei, este suficient să folosiți **intrări permanente**.
    **Pentru a nu bloca routerul, pachetele pentru care așteptăm un `ARP Reply`
    vor fi puse într-o coadă.**

    Vă recomandăm să folosiți comanda `arping` pentru a face cereri ARP. Comanda
    `arp` afișează tabela ARP a unui host.

    Tema se poate preda și cu o tabelă de ARP
    statică, cu pierderea punctajului aferent. În arhivă aveți inclusă o
    astfel de tabelă. **Prezența acestui fișier în arhivă va opri
    checkerul din a rula teste de ARP, deci nu veți primi puncte pe ele.
    Dacă ați implementat protocolul ARP, nu adăugați fișierul
    `arp_table.txt`**. 
    > Este normal ca testul forward să pice daca folositi tabela statica de ARP.

-   **Protocolul ICMP (21p).** Implementați pe router funcționalitatea descrisă
    în [secțiunea ICMP](icmp.html). Puteți folosi `ping -t 1` pentru a trimite
    un pachet cu TTL 1. Va recomandăm să testați și cu `traceroute`.

Pentru a fi punctată o temă, în README trebuie prezentată soluția voastră, pe scurt. De asemenea, trebuie menționat și ce subcerințe ați rezolvat.

**Notă:** Puteți scrie implementarea în C sau C++.

[^1]: <https://blog.apnic.net/2022/01/06/bgp-in-2021-the-bgp-table/>


