insert into timemodel(modelname, monday, tuesday, wednesday, thursday, friday, saturday, sunday) values("Mo-Fr 8h", 8, 8, 8, 8, 8, 0, 0);
insert into user(username, persno, firstname, lastname, active, timebalance, timemodel_fk) values("leandros", "1234A", "Leandros", "Athanasiadis", 1, 0, (select pk from timemodel where modelname='Mo-Fr 8h'));
insert into card(carduid, text, user_fk, active) values('9CBBFF15', "Schwarzer Mifare Classic 1k", (select pk from user where username='leandros'), 1);
