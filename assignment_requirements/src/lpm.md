# Longest Prefix Match

Pentru adresa IPv4 destinație din pachetul primit, routerul trebuie să
caute intrările din tabela de routare care descriu o rețea ce cuprinde
adresa respectivă. Este posibil ca mai multe intrări să se potrivească;
în acest caz, routerul trebuie s-o aleagă pe cea mai specifică, i.e. cea
cu masca cea mai mare. Acest criteriu de căutare se numește "Longest
Prefix Match" (LPM).

Programatic, routerul poate verifică apartenența unei adrese la o rețea,
făcând operația de `AND` pe biți și verificând că este egală cu
prefixul:

    ip.destination & entry.mask == entry.prefix

Odată ce routerul găsește toate prefixele care se potrivesc adresei
destinație din pachet, va alege intrarea din tabela de rutare
corespunzătoare celui mai lung prefix (masca cea mai mare).
