---------------------------------------------------------------------------------------------------------------------------------------
Apache-Konfiguration:

Wichtig ist es vor allem, "AllowOverride All" zu setzen. Und zwar im Apache Config-File. Sollte unter /etc/httpd/conf/* zu finden sein.

Siehe auch Readme.md im Webgui-Ordner unseres Projektes auf Github:

https://github.com/leandre84/rpntts/blob/master/rpntts-webgui.net/README.md

Sollte etwas nicht funken, bitte erstmal hier nachschauen:

http://framework.zend.com/manual/current/en/ref/installation.html

Im Prinzip habe ich alle meine Einstellungen nach diesen Vorgaben konfiguriert.
---------------------------------------------------------------------------------------------------------------------------------------
DB Zugang in Zend einrichten:

In den Ordner \rpntts\rpntts-webgui.net\config\autoload navigieren.

Dort das File "local.php" hier aus dem \doc\Webgui Ordner einfügen. User/Pass eurer lokalen DB eintragen.
---------------------------------------------------------------------------------------------------------------------------------------

Das war's, mehr sollte nicht zu tun sein. Falls ihr auf etwas zusätzliches wichtiges draufgekommen seid -> Bitte dieses Readme darum erweitern.