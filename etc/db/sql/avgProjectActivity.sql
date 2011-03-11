DROP PROCEDURE IF EXISTS `avgProjectActivity`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `avgProjectActivity`(IN project_id INTEGER, IN start_date DATETIME, IN end_date DATETIME, IN status_value INTEGER)
BEGIN
        SET @A = CONCAT("SELECT AVG(percentage) AS average
                FROM (
                        SELECT 100*
                        (
                                SELECT COUNT(*)
                                FROM service_status
                                WHERE idServiceInstance=s.id AND status=", status_value, " AND timestamp BETWEEN '", start_date, "' AND '", end_date,
                        "') / (
                                SELECT COUNT(*)
                                FROM service_status
                                WHERE idServiceInstance=s.id AND timestamp BETWEEN '", start_date, "' AND '", end_date,
                        "') AS percentage
                        FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id
                        WHERE u.idProject=", project_id, " AND u.endDate IS NULL
                ) t"
        );
        PREPARE STMT FROM @A;
        EXECUTE STMT;
        DEALLOCATE PREPARE STMT;
END
$$