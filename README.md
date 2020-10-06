<p align="center">
<img src="https://img.shields.io/github/last-commit/FantasyFactory/My-TTGO-Watch-MyBasic/tree/Mosconi.svg?style=for-the-badge" />
&nbsp;
<img src="https://img.shields.io/github/license/FantasyFactory/My-TTGO-Watch-MyBasic/tree/Mosconi.svg?style=for-the-badge" />
&nbsp;
<a href="https://www.buymeacoffee.com/Corrado" target="_blank"><img src="https://img.shields.io/badge/Buy%20me%20a%20coffee-%E2%82%AC5-orange?style=for-the-badge&logo=buy-me-a-coffee" /></a>
</p>
<hr/>

# My-TTGO-Watch "Mosconi Inside"

Software per lo smartwatch T-WATCH 2020 di LILIGO, basato sull'ottimo lavoro di Dirk Broßwick (<a href="https://github.com/sharandac">sharandac</a>) che potete trovare qui <a href="https://github.com/sharandac/My-TTGO-Watch">https://github.com/sharandac/My-TTGO-Watch</a> del quale questo progetto è un fork

Questo fork è dedicato alla memoria di Germano Mosconi ed quindi <b>ha contenuti fortemente blasfemi</b>, se la cosa urta la vostra sensibilità è un problema vostro; io vi ho avvisati.

Le principali differenze sono:
1) eliminate le app "crypto_ticker", "IRController", "powermeter"
2) aggiunta app "Image ticker": legge da web un file png e lo visualizza. Funziona di merda, ma è un inizio. Il problema è che ha bisogno comunque di una sorta di proxy installato su un server, perchè al momento legge solo http e non https (praticamente ormai qualsiasi cosa è https) quindi a parte questo il proxy prende una immagine da un url specificato, e la trasforma in un png di piccole dimensioni... allora a questo punto lo sbattimento per leggere i png non ha avuto senso: tanto valeva che il proxy mandasse direttamente l'immagine come array e ciaone.
3) funzionalità "mosconiane":
3.1) app generatore randomico di bestemmioni
3.2) al boot esclama "ma che ooooohhh!!!"
3.3) quando squilla il telefono, esclama "Ma cos'è 'sto telefono"
3.4) al boot ad ogni step è associata una immagine, che formano una sorta di animazione in sequenza. Questa parte contiene il logo di "porcodiOS" (che parte da iOS e accende a poco a poco il resto) e l'immagine del germanone.
4) tradotte in Italiano le indicazioni di OsmAnd (capirai che robona utile: i disegnini con le frecce li capiresti anche se le scritte fossero in aramaico)
5) il pulsante "Default url" nella pagina di setup del firmware update, una cosa che proporrò al master di sharandac

Poi, se proprio lo vuoi sapere <a href="https://github.com/sharandac/My-TTGO-Watch/compare/master...FantasyFactory:Mosconi">basta che confronti col master</a>

# Telegram chatgroup

C'è un gruppo Telegram, un po' morto, metà sono russi ma sono simpatici
https://t.me/TTGO_Watch

# Install

Clona questo repository e installa tutto in platformIO. Compila e flasha. Dal terminale di visual studio code puoi fare questo:

```bash
pio run -t upload
```

o semplicemente premere "build and upload" se non sei un nerdone masochista.

# Problemi noti

* the webserver crashes the ESP32 really often
* the battery indicator is not accurate, rather a problem with the power management unit ( axp202 )
* from time to time the esp32 crashes accidentally
* and some other small things

* image ticker si impalla continuamente, e quando non lo fa è comunque inutile
* tutto l'orologio si impalla che sembra di sentire il germanone


# how to use
Troppo sbatti a tradurre, impara l'inglese mona che non sei altro!

## weather app  