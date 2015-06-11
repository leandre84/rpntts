DROP PROCEDURE IF EXISTS rpntts_update_saldo;
DELIMITER //
CREATE PROCEDURE rpntts_update_saldo(IN booking1 INT, IN booking2 INT)

BEGIN

  DECLARE booking1_cnt INT;
  DECLARE booking1_user_fk INT;
  DECLARE booking1_timestamp TIMESTAMP;
  DECLARE booking1_type VARCHAR(2);
  DECLARE booking1_computed BOOLEAN;
  DECLARE booking1_link_fk INT;

  DECLARE booking2_cnt INT;
  DECLARE booking2_user_fk INT;
  DECLARE booking2_timestamp TIMESTAMP;
  DECLARE booking2_type VARCHAR(2);
  DECLARE booking2_computed BOOLEAN;
  DECLARE booking2_link_fk INT;

  DECLARE EXIT HANDLER FOR SQLEXCEPTION ROLLBACK;

  select count(*), user_fk, timestamp, type, computed, link_fk into booking1_cnt, booking1_user_fk, booking1_timestamp, booking1_type, booking1_computed, booking1_link_fk from booking where pk = booking1;
  select count(*), user_fk, timestamp, type, computed, link_fk into booking2_cnt, booking2_user_fk, booking2_timestamp, booking2_type, booking2_computed, booking2_link_fk from booking where pk = booking2;

  IF booking1 = booking2 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Go milk some goats';
  END IF;

  IF booking1_cnt != 1 OR booking2_cnt != 1 THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'No such bookings';
  END IF;

  IF booking1_user_fk != booking2_user_fk THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Users do not match';
  END IF;

  IF booking1_timestamp >= booking2_timestamp THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Booking order does not match';
  END IF;

  IF booking1_type != booking2_type THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Booking types do not match';
  END IF;

  IF booking1_computed = TRUE OR booking2_computed = TRUE OR booking1_link_fk IS NOT NULL OR booking2_link_fk IS NOT NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Booking already computed';
  END IF;

  START TRANSACTION;
    update user set timebalance = timebalance + time_to_sec(timediff((select timestamp from booking where pk = booking2), (select timestamp from booking where pk = booking1)))/3600 where pk = booking1_user_fk;
    update booking set computed = TRUE where pk in (booking1, booking2);
    update booking set link_fk = booking2 where pk = booking1;
    update booking set link_fk = booking1 where pk = booking2;
  COMMIT;
  
END //
DELIMITER ;


DROP PROCEDURE IF EXISTS rpntts_delete_booking;
DELIMITER //
CREATE PROCEDURE rpntts_delete_booking(IN booking INT)

BEGIN

  DECLARE booking_cnt INT;
  DECLARE booking_user_fk INT;
  DECLARE linked_booking INT;
  DECLARE booking_tb FLOAT;
  DECLARE booking_type VARCHAR(2);

  DECLARE EXIT HANDLER FOR SQLEXCEPTION ROLLBACK;

  select count(*), user_fk, link_fk, type into booking_cnt, booking_user_fk, linked_booking, booking_type from booking where pk = booking;

  IF booking_cnt IS NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'No such booking';
  END IF;

  START TRANSACTION;

  IF linked_booking IS NOT NULL THEN
    select time_to_sec(timediff((select timestamp from booking where pk = booking), (select timestamp from booking where pk = linked_booking)))/3600 into booking_tb;
    IF booking_tb < 0 THEN
      SET booking_tb = booking_tb * -1;
    END IF;
    update user set timebalance = timebalance - booking_tb where pk = booking_user_fk;
    update booking set computed = FALSE, link_fk = NULL where pk = linked_booking;
  END IF;

  delete from booking where pk = booking;

  IF (booking_type = 'U' OR booking_type = 'UH') AND linked_booking IS NOT NULL THEN
    delete from booking where pk = linked_booking;
  END IF;

  COMMIT;

END //
DELIMITER ;


DROP PROCEDURE IF EXISTS rpntts_update_global_saldo;
DELIMITER //
CREATE PROCEDURE rpntts_update_global_saldo()

BEGIN

  DECLARE booking1 INT;
  DECLARE booking2 INT;
  DECLARE runloop INT;
  SET runloop = 1;
  
  WHILE runloop != 0 DO
    SET booking1 = NULL;
    SET booking2 = NULL;
    select b1.pk, b2.pk into booking1, booking2 from (select pk, user_fk, timestamp, type from booking where computed=FALSE order by timestamp) b1 join (select pk, user_fk, timestamp, type from booking where computed=FALSE order by timestamp) b2 where b1.user_fk=b2.user_fk and b1.type=b2.type and date(b1.timestamp)=date(b2.timestamp) and b1.timestamp < b2.timestamp limit 1;
    IF booking1 IS NULL OR booking2 IS NULL THEN
      SET runloop = 0;
    ELSE
      call rpntts_update_saldo(booking1,booking2);
    END IF;
  END WHILE;

END //
DELIMITER ;


DROP PROCEDURE IF EXISTS rpntts_holiday_booking;
DELIMITER //
CREATE PROCEDURE rpntts_holiday_booking(IN user INT, IN day CHAR(8), IN type VARCHAR(2), IN text VARCHAR(64))

BEGIN

  DECLARE dow INT;
  DECLARE tbadjust FLOAT;
  DECLARE hours INT;
  DECLARE minutes INT;
  DECLARE starttime TIMESTAMP;
  DECLARE endtime TIMESTAMP;
  DECLARE pk1 INT;
  DECLARE pk2 INT;

  DECLARE EXIT HANDLER FOR SQLEXCEPTION ROLLBACK;

  SET starttime = str_to_date(concat(day, '080000'), '%Y%m%d%H%i%s');
  SET dow = dayofweek(str_to_date(day, '%Y%m%d'));

  IF dow = 1 THEN
    select sunday into tbadjust from user, timemodel where timemodel_fk=timemodel.pk and user.pk = user;
  END IF;
  IF dow = 2 THEN
    select monday into tbadjust from user, timemodel where timemodel_fk=timemodel.pk and user.pk = user;
  END IF;
  IF dow = 3 THEN
    select tuesday into tbadjust from user, timemodel where timemodel_fk=timemodel.pk and user.pk = user;
  END IF;  
  IF dow = 4 THEN
    select wednesday into tbadjust from user, timemodel where timemodel_fk=timemodel.pk and user.pk = user;
  END IF;
  IF dow = 5 THEN
    select thursday into tbadjust from user, timemodel where timemodel_fk=timemodel.pk and user.pk = user;
  END IF;
  IF dow = 6 THEN
    select friday into tbadjust from user, timemodel where timemodel_fk=timemodel.pk and user.pk = user;
  END IF;
  IF dow = 7 THEN
    select saturday into tbadjust from user, timemodel where timemodel_fk=timemodel.pk and user.pk = user;
  END IF;

  IF tbadjust IS NULL THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'No user or timemodel found';
  END IF;

  IF type = 'UH' THEN
    SET tbadjust = tbadjust/2;
  END IF;

  SET endtime = addtime(starttime, maketime(floor(tbadjust), (tbadjust-floor(tbadjust))*60, 0));

  START TRANSACTION;
    insert into booking(timestamp, type, text, user_fk) values(starttime, type, text, user);
    select last_insert_id() into pk1;
    insert into booking(timestamp, type, text, user_fk) values(endtime, type, text, user);
    select last_insert_id() into pk2;
    call rpntts_update_saldo(pk1,pk2);
  COMMIT;

END //


DELIMITER ;
DROP PROCEDURE IF EXISTS rpntts_saldo_per_day;
DELIMITER //
CREATE PROCEDURE rpntts_saldo_per_day(IN forday VARCHAR(8))

BEGIN

  DECLARE loop_finished BOOLEAN;

  DECLARE dow INT;
  DECLARE xholiday BOOLEAN;
  DECLARE xcomputed BOOLEAN;
  DECLARE user INT;
  DECLARE tbmonday FLOAT;
  DECLARE tbtuesday FLOAT;
  DECLARE tbwednesday FLOAT;
  DECLARE tbthursday FLOAT;
  DECLARE tbfriday FLOAT;
  DECLARE tbsaturday FLOAT;
  DECLARE tbsunday FLOAT;
  
  DECLARE saldo_cursor CURSOR FOR
  select user.pk, monday, tuesday, wednesday, thursday, friday, saturday, sunday from user, timemodel where timemodel_fk=timemodel.pk and exists (select 1 from day where day=str_to_date(forday, '%Y%m%d') and computed=FALSE);

  DECLARE EXIT HANDLER FOR SQLEXCEPTION ROLLBACK;
  DECLARE CONTINUE HANDLER FOR NOT FOUND SET loop_finished = TRUE;
  
  SET dow = dayofweek(str_to_date(forday, '%Y%m%d'));
  select holiday, computed into xholiday, xcomputed from day where day=str_to_date(forday, '%Y%m%d');

  IF dow IS NULL OR xholiday IS NULL OR xcomputed THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Could not get info for desired day';
  END IF;

  IF xcomputed = TRUE THEN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Calculations already performed for this date';
  END IF;

  START TRANSACTION;

  IF xholiday = FALSE THEN
    OPEN saldo_cursor;
    get_data: LOOP
    
    FETCH saldo_cursor INTO user, tbmonday, tbtuesday, tbwednesday, tbthursday, tbfriday, tbsaturday, tbsunday;

    IF loop_finished = TRUE THEN
      LEAVE get_data;
    END IF;

    IF dow = 1 THEN
      update user set timebalance=timebalance-tbsunday where pk=user;
    END IF;
    IF dow = 2 THEN
      update user set timebalance=timebalance-tbmonday where pk=user;
    END IF;
    IF dow = 3 THEN
      update user set timebalance=timebalance-tbtuesday where pk=user;
    END IF;
    IF dow = 4 THEN
      update user set timebalance=timebalance-tbwednesday where pk=user;
    END IF;
    IF dow = 5 THEN
      update user set timebalance=timebalance-tbthursday where pk=user;
    END IF;
    IF dow = 6 THEN
      update user set timebalance=timebalance-tbfriday where pk=user;
    END IF;
    IF dow = 7 THEN
      update user set timebalance=timebalance-tbsaturday where pk=user;
    END IF;

    END LOOP get_data;

    CLOSE saldo_cursor;
  END IF;

  update day set computed=TRUE where day=str_to_date(forday, '%Y%m%d');
  
  COMMIT;

END //
DELIMITER ;

-- schedule daily event
-- also set event_scheduler=on under [mysqld] in /etc/mysql/my.cnf
DROP EVENT IF EXISTS calculate_daily_saldo;
CREATE EVENT calculate_daily_saldo
ON SCHEDULE EVERY 1 DAY 
STARTS '2015-06-01 00:00:00'
ON COMPLETION NOT PRESERVE ENABLE
DO call rpntts_saldo_per_day(date_format(current_date(), '%Y%m%d'));
