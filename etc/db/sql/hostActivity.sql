DROP PROCEDURE IF EXISTS `hostActivity`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `hostActivity`(IN id_project INTEGER, IN id_host INTEGER, IN startDate DATETIME, IN endDate DATETIME)
BEGIN
        DECLARE done BOOLEAN DEFAULT false;
        DECLARE denominatore LONG;
        DECLARE idService INTEGER;
        DECLARE nameService VARCHAR(255);
        DECLARE cur CURSOR FOR
                SELECT s.id, s.name
                FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id
                WHERE s.idHost=id_host AND u.idProject=id_project AND u.endDate IS NULL;
        DECLARE CONTINUE HANDLER FOR NOT FOUND SET done=true;
        OPEN cur;
        REPEAT
                FETCH cur INTO idService, nameService;
                IF NOT done THEN
                        SET denominatore = (
                                SELECT COUNT(*)
                                FROM service_status
                                WHERE idServiceInstance=idService AND timestamp BETWEEN startDate AND endDate
                        );
                        IF denominatore = 0 THEN
                                SELECT idService, nameService, NULL as percentage;
                        ELSE
                                SELECT idService, nameService, (100.*
                                (
                                        SELECT COUNT(*)
                                        FROM service_status
                                        WHERE idServiceInstance=idService AND timestamp BETWEEN startDate AND endDate AND status=1
                                ) / denominatore) as percentage;
                        END IF;
                END IF;
        UNTIL done END REPEAT;
        CLOSE cur;
END
$$