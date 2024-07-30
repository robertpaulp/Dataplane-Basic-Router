# Prezentare generală

În cadrul temei vom implementa dataplane-ul unui router. Un router are două
părți:

-   Dataplane - partea care implementează procesul de dirijare propriu-zis, pe
    baza intrărilor din tabela de routare.

-   Control plane - partea care implementează algoritmii de rutare (e.g. RIP,
    OSPF, BGP); acești algoritmi distribuiți calculează intrările din
    tabela de rutare.


**Tema constă în implementarea componentei de dataplane**.
În cele ce urmează, în lipsa altor precizări, toate referințele la router din
textul temei se referă la dataplane.

**NU** va fi nevoie să implementați acești algoritmi în cadrul temei; vom lucra
strict din perspectiva dataplane-ului, pentru care tabelele de rutare sunt deja
populate.
Routerul nostru va funcționa cu o tabelă de rutare statică, primită într-un
fișier de intrare, și care nu se va schimba pe parcursul rulării.

Un router are mai multe interfețe și poate recepționa pachete pe oricare dintre
acestea. Routerul trebuie să transmită pachetul mai departe, către un calculator
sau către alt router direct conectat, în funcție de regulile din tabela de
rutare.

**Tema poate fi implementată în C sau C++**.

## Lectură tema

- [The Internet Protocol (9 min)](https://www.youtube.com/watch?v=rPoalUa4m8E&list=PLowKtXNTBypH19whXTVoG3oKSuOcw_XeW&index=8)
- [ARP: Mapping between IP and Ethernet (10 min)](https://www.youtube.com/watch?v=aamG4-tH_m8&list=PLowKtXNTBypH19whXTVoG3oKSuOcw_XeW&index=9)
- [Looking at ARP and ping packets (8 min)](https://www.youtube.com/watch?v=xNbdeyEI-nE&list=PLowKtXNTBypH19whXTVoG3oKSuOcw_XeW&index=12)
- [Hop-by-hop routing (13 min)](https://www.youtube.com/watch?v=VWJ8GmYnjTs&list=PLowKtXNTBypH19whXTVoG3oKSuOcw_XeW&index=11)
- Capitolul 4.3 (What's Inside a Router) din [Computer Networking: A Top-Down
Approach (6th
Edition)](https://eclass.teicrete.gr/modules/document/file.php/TP326/%CE%98%CE%B5%CF%89%CF%81%CE%AF%CE%B1%20(Lectures)/Computer_Networking_A_Top-Down_Approach.pdf)
- Capitolul 4.4.3 (Internet Control Message Protocol (ICMP)) din
[Computer Networking: A Top-Down Approach (6th
Edition)](https://eclass.teicrete.gr/modules/document/file.php/TP326/%CE%98%CE%B5%CF%89%CF%81%CE%AF%CE%B1%20(Lectures)/Computer_Networking_A_Top-Down_Approach.pdf).
- Capitolul 5.4.1
(Link-Layer Addressing and ARP) din [Computer Networking: A Top-Down
Approach (6th
Edition)](https://eclass.teicrete.gr/modules/document/file.php/TP326/%CE%98%CE%B5%CF%89%CF%81%CE%AF%CE%B1%20(Lectures)/Computer_Networking_A_Top-Down_Approach.pdf)

## Deadline

Deadline-ul temei este specificat pe moodle.

Vă reamintim părțile relevante din [regulamentul cursului de
PCom](https://curs.upb.ro/2023/pluginfile.php/270568/mod_resource/content/1/Regulament_PC_2023_2024.pdf):

-   După expirarea acestui termen limită se mai pot trimite teme un
    interval de maxim 3 zile, cu următoarele depunctări: 10p în prima
    zi, 20p în a două zi și 30p în a treia zi.

-   După cele 3 zile tema nu se mai poate trimite.

-   Oferim posibilitatea fiecărui student de a avea un număr de maxim
    **5 zile** numite "sleep days".

-   Aceste zile pot fi folosite pentru a amâna termenul de predare al
    temei de casă (fără penalizări).

-   Nu se pot folosi mai mult de **două** sleep days pentru o temă de
    casă.

-   Pentru a utiliza aceste zile completați formularul de pe Moodle

-   Temele de casă sunt *individuale*.

# Setup

Pentru a simula o rețea virtuală vom folosi [Mininet](http://mininet.org/).
Mininet este un simulator de rețele ce folosește în simulare implementari reale
de kernel, switch și cod de aplicații.

```bash
sudo apt update
sudo apt install mininet openvswitch-testcontroller tshark python3-click python3-scapy xterm
sudo pip3 install mininet
```

După ce am instalat Mininet, vom folosi următoarea comandă pentru a crește
dimensiunea fontului în terminalele pe care le vom deschide.

```bash
echo "xterm*font: *-fixed-*-*-*-18-*" >> ~/.Xresources 
xrdb -merge ~/.Xresources
```
