# webserv
BONHOMME

# todo
body_size_max
timeout request
chunked transfer encoding
multiple cgi extension

autoindex
redirection (return)
multiple server name
cgi bloquant
content type dynamique

# INFO EN VRAC:

## setsockopt
int opt = 1;
if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	throw std::runtime_error("setsockopt failed");

setsockopt sert à modifier les options associées à un socket.
sockfd : le descripteur du socket (celui que tu veux configurer).
level : la couche ciblée (ex. SOL_SOCKET pour les options génériques, IPPROTO_TCP pour TCP).
optname : l’option à régler (ex. SO_REUSEADDR, SO_KEEPALIVE, TCP_NODELAY...).
optval : pointeur vers la valeur à appliquer.
optlen : taille de cette valeur.

demande au kernel d'activer SO_REUSEADDR, si ca fonctionne, setsockopt renvoie 0 et on peut ensuite faire le bind correctement, sinon renvoie -1

appliquer setsockopt juste après socket() et vérifier les retours, sinon le bind suivant peut échouer

port 443 et 80 sans permissions

# Pour tester une requete chunked avec netcat
nc localhost 8080 < netcat.txt

netcat.txt:
```
POST /post_body HTTP/1.1
Host: localhost:8080
Transfer-Encoding: chunked
Content-Type: text/plain

5
Hello
8
 World !
0

```

 # Pour tester une requete chunked avec curl
 ```
 curl -v -X POST http://localhost:8080/post_body \
     -H "Transfer-Encoding: chunked" \
     -H "Content-Type: text/plain" \
     -d "Ceci est un test de message envoyé par morceaux."
```

## wait_result = waitpid(id, &status, WNOHANG);
Avec WNOHANG, l'appel à waitpid devient une simple vérification:
Si le CGI est fini : waitpid renvoie le PID de l'enfant, nettoie le processus zombie et remplit le status

Si le CGI tourne encore : waitpid renvoie immédiatement 0. Le serveur continue alors son exécution normalement

# Ressource:
https://www.garshol.priv.no/download/text/http-tut.html

CGI:
https://web.developpez.com/cgic.htm

https://www.snv.jussieu.fr/manual/fr/howto/cgi.html

infos utiles(peut etre(jsp?)):

https://stackoverflow.com/questions/26137190/what-happens-when-you-go-back-with-the-browser

https://www.youtube.com/watch?v=RpR_jEoAlxw (cgi tuto)

https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Status (http status code response)

https://stackoverflow.com/questions/8659808/how-does-http-file-upload-work (file upload)

multipart/form-data:
https://medium.com/@muhebollah.diu/understanding-multipart-form-data-the-ultimate-guide-for-beginners-fd039c04553d (multipart/form-data)

https://stackoverflow.com/questions/16958448/what-is-http-multipart-request