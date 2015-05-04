CREATE USER 'rpntts' IDENTIFIED BY 'rpntts';
CREATE DATABASE rpntts;
GRANT ALL ON rpntts.* TO rpntts;

USE rpntts;

CREATE TABLE timemodel (
    pk INTEGER PRIMARY KEY AUTO_INCREMENT,
    modelname VARCHAR(64) NOT NULL,
    monday FLOAT NOT NULL,
    tuesday FLOAT NOT NULL,
    wednesday FLOAT NOT NULL,
    thursday FLOAT NOT NULL,
    friday FLOAT NOT NULL,
    saturday FLOAT NOT NULL,
    sunday FLOAT NOT NULL
);

CREATE TABLE user (
    pk INTEGER PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(32) NOT NULL,
    password VARCHAR(64),
    persno VARCHAR(32),
    firstname VARCHAR(64) NOT NULL,
    lastname VARCHAR(64) NOT NULL,
    active INTEGER NOT NULL,
    timebalance FLOAT NOT NULL,
    timemodel_fk INTEGER NOT NULL,
    FOREIGN KEY(timemodel_fk) REFERENCES timemodel(pk)
);

CREATE TABLE card(
    carduid VARCHAR(10) PRIMARY KEY,
    text VARCHAR(64),
    user_fk INTEGER NOT NULL,
    active INTEGER NOT NULL,
    FOREIGN KEY(user_fk) REFERENCES user(pk)
);

CREATE TABLE booking(
    pk INTEGER PRIMARY KEY AUTO_INCREMENT,
    timestamp TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    type VARCHAR(2) NOT NULL,
    text VARCHAR(64) NOT NULL,
    user_fk INTEGER NOT NULL,
    computed BOOLEAN NOT NULL,
    FOREIGN KEY(user_fk) REFERENCES user(pk)
);

ALTER TABLE timemodel ADD CONSTRAINT timemodel_modelname_ux UNIQUE (modelname);
ALTER TABLE timemodel ADD CONSTRAINT timemodel_weekdays_ux UNIQUE (monday, tuesday, wednesday, thursday, friday, saturday, sunday);
ALTER TABLE user ADD CONSTRAINT user_username_ux UNIQUE (username);
ALTER TABLE user ADD CONSTRAINT user_persno_ux UNIQUE (persno);
ALTER TABLE booking ADD CONSTRAINT booking_user_time_ux UNIQUE(user_fk, timestamp);

