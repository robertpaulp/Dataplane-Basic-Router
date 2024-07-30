# Procesul de dirijare (forwarding)

Dirijarea este un proces care are loc la nivelul 3 ("Rețea") din stiva
OSI. În momentul în care un pachet ajunge la router, acesta trebuie să
efectueze următoarele acțiuni:

1.  **Parsarea pachetului:** din fluxul de octeți ce reprezintă
    pachetul, routerul trebuie să afle ce antete sunt prezente și ce
    valori conțin câmpurile acestora, construind și inițializând
    structuri interne corespunzătoare. La acest pas, dacă routerul
    descoperă că un pachet primit e malformat (e.g. prea scurt), îl
    aruncă.

2.  **Validarea L2:** pachetele conțin un antet de `Ethernet`; routerul
    nostru trebuie să considere doar pachetele trimise către el însuși
    (i.e. câmpul `MAC destination` este același cu adresa MAC a
    interfeței pe care a fost primit pachetul) sau către toată lumea
    (i.e. câmpul `MAC destination` este adresa de *broadcast*,
    `FF:FF:FF:FF:FF:FF`). Orice alt pachet trebuie aruncat.

3.  **Inspectarea următorului header:** routerul inspectează câmpul
    `Ether Type` din header-ul Ethernet pentru a descoperi următorul antet
    prezent. În cadrul temei, ne vor interesa doar două posibilități:
    [IPv4](ip.md) și [ARP](arp.md). Următoarele acțiuni ale routerului vor
    depinde de tipul acestui header. Orice alt fel de pachet trebuie
    ignorat.


> În continuare, vom studia protocoalele implicate în procesul de dirijare
și vom prezenta cum acestea sunt folosite de către router.
