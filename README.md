# bp-Remote_cache_poisoning

Tu si môžete stiahnuť virtuálne počítače a prostriedky, ktoré potrebujete na spustenie útoku na otravu vzdialenej vyrovnávacej pamäte.

## Úvod
Cieľom tohto labu je, aby študenti nadobudli prvé skúsenosti s útokom na otravu vzdialenej vyrovnávacej pamäte DNS, ktorý sa nazýva aj útok Kaminsky DNS. V súbore topology.png môžete vidieť, že toto laboratórium obsahuje 4 virtuálne stroje. Prvý je router s IP adresou 10.10.30.1, druhý je lokálny DNS server (obeť) s IP adresou 10.10.30.7, tretí je útočník (vy) s 10.10.30.6 a posledný je server-wan s 10.10 .40.40 IP adresa. Vašou úlohou je otráviť vyrovnávaciu pamäť lokálneho servera DNS falošnou odpoveďou.

## Ako funguje vzdialené otrávenie vyrovnávacej pamäte
V skutočnom svete nie sú útočník a lokálny server DNS v rovnakej sieti. Útočník tým, že nepočúva komunikáciu, nepozná zdrojovú IP adresu, cieľové číslo portu a ID transakcie. Tieto tri informácie sú pre útočníka kritické, ak chce, aby bol útok úspešný. Treba však vyriešiť dva skutočné problémy.
##### Prvým problémom je načasovanie.
Pri lokálnom útoku útočník vedel zachytiť pakety, takže presne vedel, kedy bol paket odoslaný. Pri vzdialenom útoku tohto útočníka nevidíte. Tento problém sa však dá ľahko vyriešiť. Aby útočník vedel, kedy má poslať falošné odpovede, odošle požiadavku na lokálny server DNS a spustí útok a zaplaví lokálny server DNS falošnými odpoveďami.
##### Druhým problémom je vyrovnávacia pamäť lokálneho servera DNS.
Ak útok nie je úspešný a legitímnemu NS sa podarí odpovedať, odpoveď sa uloží do vyrovnávacej pamäte. Ak by útočník pokračoval, útok by už nemal zmysel, pretože pri ďalšom pokuse sa na NS webu neodošle žiadna požiadavka, ale miestny DNS server prevezme odpoveď zo svojej vyrovnávacej pamäte. Útočník by teda musel čakať na vypršanie platnosti TTL, aby sa záznam stal neplatným. V takom prípade by lokálny server DNS musel požiadavku znova odoslať a útočník by mal možnosť vykonať útok, ale opäť len jeden pokus. Ak by neuspel, musel by znova čakať na vypršanie TTL, mal jeden pokus atď. Do uplynutia platnosti TTL môžu uplynúť dni. Ak by teda útočník mal vždy len jednu príležitosť v priebehu niekoľkých dní, napríklad dva alebo tri, potom by samotný útok musel trvať desaťročia alebo stovky, aby bol úspešný. Z tohto dôvodu bol vzdialený útok v praxi nepraktický.

#### Útok Dana Kaminského
Dan prišiel s veľmi šikovným nápadom. Namiesto toho, aby sa útočník stále opýtal jednu otázku (stuba.sk), položí ďalšiu, napríklad a.stuba.sk. Útočník s najväčšou pravdepodobnosťou prehrá a lokálny DNS server dostane legitímnu odpoveď od skutočného menného servera. Ak názov na serveri názvov neexistuje, lokálny server DNS dostane odpoveď, že názov neexistuje a uloží tieto informácie do medzipamäte. Takže a.stuba.sk bude cachovaná, buď so skutočnou IP adresou alebo so záznamom, ktorý hovorí, že toto meno neexistuje. To je v poriadku, pretože útočník už nepoloží tú istú otázku, ale teraz pošle b.stuba.sk. Ak je záznam aj kešovaný, útočník môže pokračovať, c.stuba.sk, d, e, f, atď. Vždy sa pýta ďalšiu otázku, takže odpoveď na ňu sa neuloží. Lokálny server DNS bude musieť odosielať požiadavky, takže útočník nemusí čakať na vypršanie platnosti záznamu TTL uloženého vo vyrovnávacej pamäti. Pri tomto útoku sa útočník zámerne nezameriava na sekciu odpovedí. Dôležitá je sekcia autority. Ak je útok úspešný, názvový server útočníka sa uloží do vyrovnávacej pamäte na lokálnom serveri DNS ako autorita pre doménu. V tomto bode je vyrovnávacia pamäť infikovaná a doména je napadnutá útočníkmi
## Inštalácia
Najprv musíte skopírovať toto úložisko do počítača. Po stiahnutí prejdite do priečinka a prejdite do priečinka **muni-kypo_VMs**. Spustite tento príkaz:
<br />
*create-sandbox --provisioning-dir ./provisioning ./remote.yml*
<br />
toto by malo vytvoriť prechodnú definíciu karantény. Prejdite do priečinka **sandbox** a spustite tento príkaz:
<br />
*spravovať vytvorenie karantény*
<br />
po chvíli sa vo virtuálnom boxe zobrazia tri virtuálne stroje - router, server-lan a útočník.
<br />
Teraz musíte zostaviť posledný - Local_DNS_server. Prejdite do priečinka **vagrant_VMs/client**, ktorý sa nachádza v stiahnutom priečinku. Spustite tento príkaz:
<br />
*tulák hore*
<br />
po chvíli by ste mali vo virtuálnom boxe vidieť Local_DNS_server. Prvýkrát tento stroj vypnete a znova zapnete. Je to dôležité, pretože toto zariadenie sa prvýkrát nepripojí k sieti.

## Úlohy
Najprv musíte spustiť bash skript na klientovi Local_DNS_server. Otvorte terminál na Local_DNS_server a spustite check_attacker.sh. <br />
`./check_attacker.sh` <br />
Tento skript vypíše vyrovnávaciu pamäť každých 60 sekúnd a skontroluje či je útočník NS vo vyrovnávacej pamäti a teda či bol samotný útok úspešný. <br /> 
**Pozor** <br />
Po 15 minútach sa obnovia pravidlá firewallu a daný útok už nebude možné uskutočniť!
<br /><br />
Na útočnom stroji:
1. Prejdite do adresára remote_repo. V tomto adresári sú zobrazené všetky súbory a prostriedky, ktoré budete potrebovať na to aby bol útok úspešný. <br />
    `cd remote_repo`
2. Skopírujte obsah _etc_bind_attacker+example do /etc/bind/named.conf. Týmto vytvoríte dve zóny na serveri DNS. Oba tieto súbory zóny sa použijú na iteratívne vyhľadávanie (názvu hostiteľa na IP adresu). <br />
    *príkazy napríklad:* <br />
    `cat _etc_bind_attacker+example` -> skopírujte obsah súboru <br />
    `sudo vi /etc/bind/named.conf` -> vložte obsah do tohto súboru
3. Skopírujte attacker.com.zone do priečinka /etc/bind. Toto je zóna vyhľadávania vpred pre doménu útočník32. Tu je uložené skutočné rozlíšenie DNS. Čitatelia, ktorí sa zaujímajú o syntax súboru zóny, si môžu pozrieť podrobnosti v RFC 1035. <br />
    `sudo cp attacker.com.zone /etc/bind/`
4. Skopírujte example.com.zone do priečinka /etc/bind. Toto je zóna vyhľadávania dopredu, napríklad doména (falošná). <br />
    `sudo cp example.com.zone /etc/bind/`
5. Reštartujte službu bind9 a skontrolujte, či je spustená funkcia bind9. Pri každej zmene konfigurácie DNS je potrebné reštartovať server DNS. <br />
    `sudo service bind9 restart`
6. Doplňte chýbajúce miesta v súbore request.py, spravte súbor request.py spustiteľným a spustite súbor request.py. Na úpravu tohto súboru použite svoj obľúbený textový editor (vyplňte miesta na ktorých sú hviezdičky) a potom <br />
    `sudo chmod +x request.py` -> urobte ho spustiteľným <br />
    `sudo ./request.py` -> spustiť <br />
po spustení skriptu python vo vašom priečinku sa zobrazí nový súbor bin. Tento súbor bin bude použitý kódom C na generovanie falošnej DNS požiadavky (dotazu). <br />
7. Doplňte chýbajúce miesta v súbore reply.py, nastavte súbor reply.py na spustiteľný a spustite súbor reply.py. Na úpravu tohto súboru použite svoj obľúbený textový editor (vyplňte miesta označené hviezdičkami) a potom <br />
    `sudo chmod +x reply.py` -> urobte ho spustiteľným <br />
    `sudo ./reply.py` -> spustiť <br />
 po spustení skriptu python vo vašom priečinku sa zobrazí nový súbor bin. Tento súbor bin bude použitý kódom C na generovanie falošnej DNS odpovede. <br />
8. Kompilujte útok.c. Ak použijete príkaz uvedený nižšie, zostavený súbor bude mať názov *a.out* <br />
    `sudo gcc attack.c`
9. Spustite skompilovaný súbor (nezabudnite ho spustiť ako sudo). Je to dôležité, pretože ak ho nespustíte ako sudo, virtuálny stroj nemusí odosielať pakety. <br />
    `sudo ./a.out` <br />
**Výsledok** <br />
Teraz prejdite na stroj Local_DNS_server a sledujte výstup spusteného skriptu, ktorý ste spustili na začiatku. Ak správne vyplníte python kódy, mali by ste vidieť správu "Útok bol úspešný" a tiež záznam NS útočník32.com.

## Zdroje
https://seedsecuritylabs.org/Labs_16.04/PDF/DNS_Remote_new.pdf
