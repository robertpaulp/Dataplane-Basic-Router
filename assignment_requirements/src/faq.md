# FAQ & probleme

-   **Q:** Pe local am mai multe puncte decât pe checkerul online

    **A:**
    * Problema poate apărea atunci când implementarea voastră are o
    performanță scăzută (e.g. faceți sortare la fiecare apel de LPM).
    Pentru a rezolva problema, asigurați-vă că aveți o implementare bună
    din punct de vedere al performanței.
    * Verificați ca tabela de rutare să fie conform celei din schelet
    * Aveți funcții care vă scad performanța codului (e.g. printf)
    * Aveți undefined behaviour care este vizibil doar pe checker. În acest caz ar trebui să folosiți valgrind și address sanitization pentru a îl detecta
    * Ați uitat să includeți tabela ARP statică în arhivă în cazul în care o folosiți

-   **Q:** Cum pornesc mai mult de un terminal?

    **A:** rulați în background.

        xterm &

-   **Q:** Primesc următoarea eroare:

        Exception: Please shut down the controller which is running on port 6653

    **A:** în cazul în care portul este ocupat rulați sudo fuser -k
    6653/tcp

-   **Q:** Cand rulez checker-ul primesc o eroare legata de lispa fisierelor `router0.pcap` si `router1.pcp`.

    **A:** Nu a fost instalat tshark: `apt install tshark`



-   **Q:** Cum extrag header-ul IP dintr-un pachet de tip msg?

    **A:**

        struct iphdr *ip_hdr = (struct iphdr *)(packet.payload + sizeof(struct ether_header));

-   **Q:** Cum pot vedea traficul de pe interfață X?

    **A:** Folosind `Wireshark`

-   **Q:** Cum afisez interfețele unui host?

    **A:** `ip address show`


-   **Q:** Valorile observate într-o captură de rețea nu coincid cu cele
    emise.

    **A:** Cel mai probabil este o problemă de endianness. Câmpurile
    unor pachete în tranzit trebuie să fie în forma "Network Byte Order"
    (care este de fapt big endian), pe când mașinile voastre sunt foarte
    probabil little endian. Folosiți următoarele funcții pentru a obține
    endiannessul corect.

    ```c
    uint32_t htonl(uint32_t);     /* host to network long */
    uint16_t htons(uint16_t);     /* host to network short */
    uint32_t ntohl(uint32_t);     /* network to host long */
    uint16_t ntohs(uint16_t);     /* network to host short */
    ```

-   **Q:** Cum inițializez o coadă?

    **A:**

        queue q;
        q = queue_create();

-   **Q:** Cum adaug un element în coadă?

    **A:**

        queue_enq(q, packet);

-   **Q:** Am implementat tot și nu îmi trec testele.

    **A:**

    * În cazul în care aplicați diferite operații asupra tabelei de rutare
    la etapă de preprocesare, aveți grijă ca acestea să aibă o
    complexitate O(n log(n)) deoarece testarea temei începe după două secunde
    de inițializare.
    * Verificați ca tabela de rutare să nu fi fost modificată

-   **Q:** Cum determin adresa IP și MAC-ul unei interfețe a routerului?

    **A:**

    ```c
    /* Întoarce adresa în format zecimal, e.g. "192.168.2.1" */
    char *get_interface_ip(int interface);
    /* Adresa e întoarsă prin parametrul de ieșire mac; sunt întorși octetii. */
    void get_interface_mac(int interface, uint8_t *mac);
    ```

-   **Q:** Nu văd output de la router (`router_output.txt` este gol).

    **A:** Probabil routerul primește segmentation fault înainte să dea flush la output;
    setați `stdout` să fie unbuffered.

    ```c
    /* fist instruction în main from router.c */
    setvbuf(stdout, NULL, _IONBF, 0);
    ```

-   **Q:** Când rulez `make` primesc o eroare ce conține \"cannot open
    output file router: No such file or directory\".

    **A:**

        $ make clean
        $ make

-   **Q:** În WireShark văd "Internet Protocol, bogus version".

    **A:** Completarea antetului IP/ICMP nu este corectă.

-   **Q:** Putem folosi C++?

    **A:** Da.

-   **Q:** Ce biblioteci sunt permise?

    **A:** Este permisă folosirea oricărei funcții din biblioteca
    standard de C sau C++. Mai mult, puteți folosi orice header standard
    de Linux, cu precizarea că trebuie în continuare să rezolvați manual
    cerința. De exemplu, Linux știe să răspundă singur la ICMP; pe
    router această funcționalitate este dezactivată de framework-ul de
    testare. Reactivarea ei cu apeluri din C nu reprezintă o soluție
    validă și nu va fi punctată.

-   **Q:** Primesc eroarea "\[14\]: ioctl SIOCGIFINDEX No such device"
    când încerc să rulez pe mașina virtuală de Linux.

    **A:** Routerul caută anumite interfețe după nume; acestea probabil
    nu există pe sistemul vostru, sunt create de mininet. Asigurați-vă
    că rulați binarul de router dintr-un terminal de router după
    pornirea topologiei.

-   **Q:** Când încerc să accesez informația din antetul ICMP, în timp
    ce dau ping de la un host la altul, toate informațiile din header
    sunt 0.

    **A:** Spre deosebire de laborator, în temă trebuie implementat și
    protocolul ARP. În cazul de față, se încearcă extragerea antetului
    ICMP dintr-un pachet ce conține doar Ethernet + ARP.

-   **Q:** Cum pot determina din cod adresa MAC a unei interfețe?

    **A:** Aveți o funcție ajutătoare `get_interface_mac`, care primește
    o interfață și-i întoarce adresa MAC. Interfața pe care a venit
    pachetul trebuie extrasă din structura `msg`.

-   **Q:** La testul "timeout is unreachable" primesc următoarea eroare:

        File "./checker.py", line 38, în passive
        status = fn(testname, packets)
        File "/media/sf_Shared_Folder/PC/tema1/tests.py", line 351, în icmp_timeout_p
        assert ICMP în packets[1], "no ICMP packet from router"

    **A:** Nu trimiți înapoi un pachet de tip ICMP (probabil nu setați
    corect câmpul protocol din headerul IP).

-   **Q:** Îmi pică ultimele 2 teste.

    **A:** Ultimele două teste o să pice, în general, dacă realizați LPM
    ineficient.

-   **Q:** Nu îmi apar terminalele când rulez mininet.

    **A:** probabil nu aveți `xterm` instalat.

        sudo apt install xterm

-   **Q:** Dacă aveți eroarea de mai jos trebuie să instalați
    openvswitch-testcontroller și creat fișierul /usr/bin/ovs-controller

        raise Exception( 'Could not find a default OpenFlow controller' )

