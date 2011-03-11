DROP PROCEDURE IF EXISTS `avgServiceActivity`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `avgServiceActivity`(IN project_id INTEGER, IN service_id INTEGER, IN start_date DATETIME, IN end_date DATETIME)
BEGIN
        SET @A = CONCAT("SELECT 100*
        (
                SELECT COUNT(*)
                FROM service_status
                WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '", start_date, "' AND '", end_date,
        "') / (
                SELECT COUNT(*)
                FROM service_status
                WHERE idServiceInstance=s.id AND timestamp BETWEEN '", start_date, "' AND '", end_date,
        "') AS percentage
        FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id
        WHERE s.id=", service_id," AND u.endDate IS NULL AND u.idProject=", project_id);
        PREPARE STMT FROM @A;
        EXECUTE STMT;
        DEALLOCATE PREPARE STMT;
END
$$