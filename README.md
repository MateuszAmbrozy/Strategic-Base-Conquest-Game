# TietoEVRY
## Plik status.txt
Plik status składa się z lini określającej ilość złota obu graczy i opisu jednostek.<br>
Plik status jest odczytywany przez program player1 lub player2, zawsze jest przygotowany pod program,<br>
który to będzie wczytywał, złoto dotyczy gracza który wczytuje plik. <br>
Plik status jest dostosowywany przez program mediator do playera<br>
z oznaczeniami P i E oraz wysyłana jest tylko ilość złota przypisana do danego gracza<br>
## Liczenie tur do zakończenia gry
Przy każdym uruchomieniu programu Player1 lub Player2 licznik tur jest zwiększany i zapisywany do pliku endGame.txt<br>
## Tworzenie jednostek
Jednostki są tworzone w trakcie trwania programu graczy. Po wysłaniu rozkazu budowania do mediatora od następnej<br>
tury czas jest odliczany i po odliczeniu czasu budowy jednostki (klasa Stoper jest odpowiedzialna za odliczanie czasu budowy) <br>
jest ona dodawana do pozostałych jednostek z odpowiednią przynależnością do gracza.<br>
Czasy budowy jednostki są zapisywane w pomocniczych plikach o takiej samej nazwie<br>
jak nazwa programu gracza + roszerzenie .txt. Aby możliwe było odliczanie czasu budowania przyjacielskiej jednostki <br>
również w turze przeciwnika pliki zawierające pozostały czas muszą być wykorzystywane w innych programach.<br>
Gdyby pliki te były prywatne, to odliczanie czasu w turze przciwnika nie byłoby możliwe.


