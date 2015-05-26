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

  DECLARE EXIT HANDLER FOR SQLEXCEPTION ROLLBACK;

  select count(*), user_fk, link_fk into booking_cnt, booking_user_fk, linked_booking from booking where pk = booking;

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

  COMMIT;

END //
DELIMITER ;
