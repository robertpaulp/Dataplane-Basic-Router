# Protocolul IPv4

Protocolul IP este utilizat pentru a permite dispozitivelor conectate în rețele
diferite să schimbe informații prin intermediul unui dispozitiv intermediar
numit router.
O descriere completă a header-ului de IPv4 o găsiți
[aici](https://en.wikipedia.org/wiki/Internet_Protocol_version_4#Header).
Header-ul unui pachet (packet) IP este următorul:

```
+----+---------------+---------------+---------------+---------------+
|Word|       1       |       2       |       3       |       4       |
+----+---------------+---------------+---------------+---------------+
|Byte|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   0|Version|  IHL  |Type of Service|          Total Length         |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   4|         Identification        |Flags|      Fragment Offset    |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   8|  Time to Live |    Protocol   |         Header Checksum       |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  12|                       Source Address                          |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  16|                    Destination Address                        |
+----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

Următoarea strucură poate fi folosită pentru a reprezenta un pachet IPV4.

```C
struct iphdr {
    // version este pe 4 biți, ihl este pe 4 biți
    uint8_t    version:4,
               ihl:4;     // Nu vom implementa protocolul IP cu opțiuni,
                          // așa că 5 × 32 bits = 160 bits = 20 bytes
                          // (internet header length)
    uint8_t    tos;         // Nu este relevant pentru temă (set pe 0)
    uint16_t   tot_len;     // dimensiunea totală a header + date
    uint16_t   id;          // Nu este relevant pentru temă, (set pe 1)
    uint16_t   frag_off;    // Nu este relevant pentru temă, (set pe 0)
    uint8_t    ttl;         // Time to live
    uint8_t    protocol;    // Identificator al protocolului encapsulat (e.g. ICMP)
    uint16_t   check;       // checksum of the iphdr, we checksum = 0 when computing
    uint32_t   saddr;       // Adresa IP sursă
    uint32_t   daddr;       // Adresa IP destinație
};
```

**Nu suntem interesați de anumite funcționalități din IP precum fragmentarea**.
Astfel, în cazul în care construiți un pachet IP de la 0,
vom seta următoarele:

```
tos = 0
frag_off = 0
version = 4
ihl = 5
id = 1
```

În [RFC 990 pagina 24](https://www.rfc-editor.org/rfc/rfc990) gășiți o listă de identificatori pentru diversele
protocoale ce pot fi encapsulate în IP. Vom folosi aceste valori pentru a completa câmpul `protocol`.

Când un router primește un pachet de tip IPv4, trebuie să realizeze
următoarele acțiuni:

1.  **Verifică dacă el este destinația:** deși un intermediar, routerul este
    de asemenea o entitate cu interfețe de rețea și adrese IP asociate acestora,
    deci poate fi destinatarul unui pachet. În acest caz, routerul nu trebuie să
    trimită mai departe pachetul, ci să îi înțeleagă conținutul pentru a putea
    acționa în consecință. În cadrul acestei teme, routerul va răspunde doar
    la mesaje de tip **ICMP**.

2.  **Verifică checksum:** routerul trebuie să recalculeze suma de
    control a pachetului, și să o compare cu cea primită în antetul de IP; dacă
    sumele diferă, pachetul a fost corupt și trebuie aruncat.

3.  **Verificare și actualizare `TTL`:** pachetele cu câmpul `TTL` având
    valoarea 1 sau 0 trebuiesc aruncate. Routerul va trimite înapoi,
    către emițătorul pachetului un mesaj ICMP de tip "Time exceeded"
    (mai multe detalii în [secțiunea ICMP](#subsect:icmp)). Altfel,
    câmpul `TTL` e decrementat.

4.  **Căutare în tabela de rutare:** routerul caută adresa IP destinație
    a pachetului în tabela de rutare pentru a determina adresa
    următorului hop, precum și interfața pe care va trebui scos
    pachetul. În caz că nu găsește nimic, pachetul este aruncat.
    Routerul va trimite înapoi, către emițătorul pachetului un mesaj
    ICMP de tip "Destination unreachable" (mai multe detalii în
    [secțiunea ICMP](#subsect:icmp)).

5.  **Actualizare checksum:** routerul recalculează suma de control a
    pachetului (această trebuie recalculată din cauza schimbării
    câmpului `TTL`) și o surpascrie în antetul IP al pachetului.

6.  **Rescriere adrese L2:** pentru a forma un cadru corect care să fie
    transmis la următorul hop, routerul are nevoie să rescrie adresele
    de L2: adresa sursă va fi adresa interfeței routerului pe care
    pachetul e trimis mai departe, iar adresa destinație va fi adresa
    MAC a următorului hop. Pentru a determina adresa următorului hop,
    routerul folosește protocolul [ARP](#subsect:arp).

7.  **Trimiterea noului pachet pe interfața corespunzătoare următorului hop.**

