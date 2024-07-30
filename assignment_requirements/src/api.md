# API

Pentru rezolvarea temei vă punem la dispoziție un schelet de cod care
implementeze unele funcționalități esențiale pentru rezolvarea cerințelor,
precum și unele funcții ajutătoare a căror utilizare este opțională. În
**protocols.h** găsiți structuri pentru protocoalele cu care vom interacționa. În
**lib.h** veți găsi mai mulți funcții auxiliare utile, printre care:

-   **Recepționare/trimitere pachete:** aveți la dispoziție următoarele
    două funcții de la nivelul Datalink:
    ```c
    /* Scrie în frame_data continutul unui cadru L2 de Ethernet. În cazul nostru, frame_data
       va fi structurat astfel:

       |Alte protocoale encapsulate de IP (e.g. ICMP)|
       |IP or ARP|
       |Ethernet L2 frame| |<IP or ARP>|

       Returnează interfața pe care cadrul a fost primit. Funcția este blocantă.
     */
    int recv_from_any_link(char *frame_data, size_t *length);

    /* Trimite frame_data ca payload al unui cadru Ethernet L2.
       Link-ul pe care îl va trimite este identificat de interfață.
     */
    int send_to_link(int interface, char *frame_data, int length);
    ```

    > Pentru a primi punctajul pe temă, trebuie să folosiți acest API de send/recv

-   **Intrări în tabela de routare:** puteți modela o intrare în tabela
    de routare folosind următoarea structură:
    ```c
    /* Route table entry */
    struct route_table_entry {
        uint32_t prefix;
        uint32_t next_hop;
        uint32_t mask;
        int interface;
    } __attribute__((packed));
    ```

-   **Parsare tabela de routare:** pentru a parsa tabela de routare,
    puteți folosi funcția:

    ```c
    int read_rtable(const char *filepath, struct route_table_entry *rtable);
    ```

    > Intrările din tabela de rutare sunt deja in network order.

-   **Intrări în tabela ARP:** puteți modela o intrare în tabela ARP
    folosind următoarea structură:

    ```c
    struct arp_table_entry {
        uint32_t ip;
        uint8_t mac[6];
    };
    ```

-   **Parsare tabela statică ARP:** în cazul în care doriți să folosiți
    tabela statică de ARP, puteți să o parsați folosind funcția:

    ```c
    int parse_arp_table(char *path, struct arp_table_entry *arp_table);
    ```

-   **Calcul sume de control:** pentru a realiza calcularea/verificarea
    sumelor de control din IPv4, respectiv ICMP, puteți folosi
    următoarea funcție:

    ```c
    /* Atunci cand calculam checksum-ul header-ului, vom pune
       campul checksum din header pe 0. */
    uint16_t checksum(uint16_t *data, size_t len);
    ```

-   **Coadă de pachete:** după cum este menționat în [descrierea
    protocolului ARP](#subsect:arp), veți avea nevoie să folosiți o
    coadă pentru pachete. Vă punem la dispoziție implementarea unei cozi
    cu elemente generice; urmăriți comentariile din `include/queue.h`.

-   **Determinarea MAC interfață proprie:** pentru a determina adresa
    MAC a unei interfețe a routerului, folosiți funcția:
    ```c
    void get_interface_mac(int interface, uint8_t *mac);
    ```

    Argumentul `mac` trebuie să indice către o zonă de memorie cu cel puțin șase
    octeți alocați.

