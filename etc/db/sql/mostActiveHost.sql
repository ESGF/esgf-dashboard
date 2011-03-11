DROP PROCEDURE IF EXISTS `mostActiveHost`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `mostActiveHost`(IN project_id INTEGER, IN start_date DATETIME, IN end_date DATETIME, IN lb INTEGER, IN size INTEGER, IN sortField VARCHAR(20), IN ascendant BOOLEAN)
BEGIN
        DECLARE dir CHAR(4);
        IF ascendant THEN
                SET dir="ASC";
        ELSE
                SET dir="DESC";
        END IF;
        SET @A = CONCAT("SELECT t.idHost, h.name, h.ip, AVG(percentage) AS average
        FROM (
                SELECT s.idHost, 100*
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
                WHERE u.idProject=", project_id,
        " AND u.endDate IS NULL) t INNER JOIN host h ON h.id=t.idHost
        GROUP BY t.idHost
        ORDER BY ", sortField, " ", dir, "
        LIMIT ", lb, " , ", size);
        PREPARE STMT FROM @A;
        EXECUTE STMT;
        DEALLOCATE PREPARE STMT;
END
$$