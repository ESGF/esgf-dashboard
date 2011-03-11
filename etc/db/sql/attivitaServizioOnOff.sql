DROP PROCEDURE IF EXISTS `attivitaServizioOnOff`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `attivitaServizioOnOff`(IN id INTEGER, IN dataInizio DATETIME, IN dataFine DATETIME, IN step LONG)
BEGIN
        DECLARE position DATETIME;
        SET position = dataInizio;
        IF step>0 THEN
                WHILE position < dataFine DO
                      (
                        SELECT 1 as status, count(*) as hitCount FROM service_status
                        WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second
                        AND status=1
                      )
                      UNION
                      (
                        SELECT -1 as status, count(*) as hitCount FROM service_status
                        WHERE idServiceInstance=id AND `timestamp` between position AND position+INTERVAL step second
                        AND status<>1
                      );
                SET position = position + INTERVAL step second;
                END WHILE;
        END IF;
END
$$