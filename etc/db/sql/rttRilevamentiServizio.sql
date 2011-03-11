DROP PROCEDURE IF EXISTS `rttRilevamentiServizio`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `rttRilevamentiServizio`(IN id INTEGER, IN dataInizio DATETIME, IN dataFine DATETIME, IN step LONG)
BEGIN
        DECLARE position DATETIME;
        SET position = dataInizio;
        IF step>0 THEN
                WHILE position < dataFine DO
                        SELECT AVG(elapsedTime) as avgRtt FROM service_status
                        WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second;
                SET position = position + INTERVAL step second;
                END WHILE;
        END IF;
END
$$