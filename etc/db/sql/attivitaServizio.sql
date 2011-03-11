DROP PROCEDURE IF EXISTS `attivitaServizio`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `attivitaServizio`(IN id INTEGER, IN startDate DATETIME, IN endDate DATETIME, IN step LONG)
BEGIN
        DECLARE position DATETIME;
        SET position = startDate;
        IF step>0 THEN
                WHILE position < endDate DO
                        SELECT `status`*1 as status, count(*) as hitCount FROM service_status
                        WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second
                        GROUP BY `status`;
                SET position = position + INTERVAL step second;
                END WHILE;
        END IF;
END
$$