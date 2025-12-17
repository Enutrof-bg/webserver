# webserv
BONHOMME


#INFO EN VRAC:

##setsockopt
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

#Ressource:
CGI:
https://web.developpez.com/cgic.htm
https://www.snv.jussieu.fr/manual/fr/howto/cgi.html

infos utiles(peut etre(jsp?)):
https://stackoverflow.com/questions/26137190/what-happens-when-you-go-back-with-the-browser