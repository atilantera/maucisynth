Tehtävää aika- ja riippuvuusjärjestyksessä:

## Verhokäyrä omaan luokkaansa

    ADSR-generaattori kannattaisi laittaa omaan luokkaansa.

    Kun katsoo esim. MainOscillator::applyAttack(),
    huomaa, että siinä tehdään useampaa asiaa:
    - puskurin loppumisen haistelu
    - ADSR-vaiheen vaihtumisen haistelu
    - itse käyrän generointi
    - outputBufferin kertominen käyrällä

    Vaikka koodi toimii, se on toisteista:
    - jokaiselle ADSR-vaiheelle on oma metodi
    - jokaisessa metodissa on if-else-lohko

    Parempi ratkaisu olisi erillinen ADSR-olio, jolta voi vain
    pyytää N samplea käyrää.

    Omana toimintonaan olisi sitten oskillaattorin raakasignaalin kertominen
    ADSR-käyrän amplitudilla.

    Tässä olisi sekin hyöty, että jatkossa voisi tuottaa ADSR-käyrän myös
    suodattimelle.

## Eksponentiaalisen verhokäyrän tuottaminen iteratiivisesti

    Tällä hetkellä eksponentiaalinen verhokäyrä tuotetaan expf()-funktiolla:

    	for (; i < rangeEnd; i++) {
			outputBuffer[i] *= b * (1 - expf(t));
			t += tIncrease;
		}

    Ongelmat:
    - kaksi kulkijamuuttujaa, i ja t
    - expf() (tulee cmath-otsakkeesta) ei ole aivan ilmainen

    Tämän pitäisi riittää:

       for (f = jotain; i < rangeEnd; i++) {
           outputBuffer[i] *= f;
           f *= k;
       }
    
    Kun 0 < k < 1, tästä tulee laskeva eksponenttikäyrä.

## GUIssa mahdollisuus valita lineaarinen tai eksponentiaalinen ADSR-käyrä


## Kunnollinen MIDI-tuki:
   MainOscillatoriin/Synthesizeriin pitää saada viivästytetty Note On ja
   Note On (viivästys n samplea): vasta sen jälkeen envelope alkaa muuttua.
   Tämä luultavasti saa tremolot (saman nuotin nopea toisto) toimimaan.
         
   Synthesizeriin tremolotunnistin: kunkin puskurillisen aikana:
    * laske, kuinka monta minkäkin sävelkorkeuden nuottia tullut
    * niille sävelkorkeuksille, joille nuotteja on enemmän kuin yksi, tehdään
      erikoiskäsittely
    * muille peruskäsittely
    * jos tulee uusi nuotti eikä vapaita oskillaattoriryhmiä ole, nuotti
      hylätään.
   
   Tällä hetkellä on tehty:
   * oskillaattorien äänengenerointiin: generoi vain puskurin samplet [a..b]
   * MIDI-nuottien lajittelu taulukoihin sortedNoteTime ja sortedNoteVelocity
     (Synthesizerin metodeissa processMidiEvents, processNoteOn ja
      processNoteOff)
     
   Seuraavaksi olisi hyvä testiprintata sortedNoteTime ja sortedNoteVelocity.   
   Käytä 4096 samplen puskurikokoa, testi-mididataa Rosegardenilla.
   (Yeah, TOIMII.)
   
   Jipii, nuottien alkuajat määriytyvät oikein ja ääni kuuluu!
   Kuitenkin äyttää siltä, että note offit eivät löydä oikeaa
   oskillaattoriryhmää.
   Seuraavaksi: katso tuloste.txt.
 
   
   
  
   

## GDB:llä ajaessa näkyy neljä säiettä.
    Tutki, mikä säie on mikin, ja
    päivitä doc/developer_introduction.txt.

## LowpassFilter
   * Lisää vielä rätisemätön rajataajuuden muutos.
   * Lisää mahdollisuus ottaa suodatin kokonaan pois käytöstä
     (jos joku haluaa tosi kirkasta ääntä).
     
   Teoriaa: nyt alipäästösuodatin ottaa vain keskiarvoja. Periaatteessa
   ylempien taajuuksien laskostumista ei pitäisi tapahtua (eihän kohinaakaan
   kuulu), mutta sinc-funktio konvoluution perustana olisi silti parempi.


## Lisää main volume -säädin GUIhin. Kohde: SynthParameters.mainVolume.  

## Säädettävä MIDI-kanava (1..16), jolla syna reagoi
    * Tämä on jo olemassa: Synthesizer::midiInChannel.
    Pitää vain tehdä lisäys GUIhin.
    
## Kanavan äänenvoimakkuus ja panorointi -kontrollerit

## Pitch bend?

# Pidemmällä tähtäimellä:

## normaalijakaumasatunnaisuus

## kolme oskillaattoria + kolme LFO:ta per nuotti

## jännempi suodatin (miten olisi konvoluutio sinc-funktiolla?)

## modulaatio-inputti JACKin kautta
   (esim. suodatettu puhe moduloisi oskillaattoreita)



