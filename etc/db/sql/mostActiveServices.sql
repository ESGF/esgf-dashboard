DROP PROCEDURE IF EXISTS `mostActiveServices`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `mostActiveServices`(IN project_id INTEGER, IN host_id INTEGER, IN start_date DATETIME, IN end_date DATETIME, IN lb INT, IN size INT, IN sortField VARCHAR(20), IN ascendant BOOLEAN)
BEGIN
        DECLARE dir CHAR(4);
        IF ascendant THEN
                SET dir="ASC";
        ELSE
                SET dir="DESC";
        END IF;
        SET @A = CONCAT("SELECT s.id, s.name, s.port, 100*
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
        WHERE s.idHost=", host_id," AND u.idProject=", project_id, " AND u.endDate IS NULL
        ORDER BY ", sortField, " ", dir, "
        LIMIT ", lb, " , ", size);
        PREPARE STMT FROM @A;
        EXECUTE STMT;
        DEALLOCATE PREPARE STMT;
END
$$