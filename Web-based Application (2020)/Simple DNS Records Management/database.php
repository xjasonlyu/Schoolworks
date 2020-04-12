<?php
class DNS_DB
{
    // class connection
    private $conn;
    // database config
    private $config;
    // default DNS table name
    private $table_name = "dns_records";

    function __construct($config)
    {
        $this->config = $config;
        // connect
        $this->conn ?? $this->connect_db();
        // create table if not exist
        $this->create_table();
    }

    function __destruct()
    {
        // if conn is not null, then close it
        !isset($this->conn) ?: $this->conn->close();
    }

    // connect_db would connect to the mysql firstly,
    // then it check if database is existed or not,
    // if not, it would create one and then use it as
    // the default database.
    private function connect_db()
    {
        $conn = new mysqli(
            $this->config['DB_HOST'],
            $this->config['DB_USER'],
            $this->config['DB_PASS']
        );
        // check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }

        $this->conn = $conn;

        // select database
        if (!mysqli_select_db($conn, $this->config['DB_NAME'])) {
            // create database if not exist
            $this->create_db();
        }
    }

    // create database
    private function create_db()
    {
        $sql = "CREATE DATABASE " . $this->config['DB_NAME'];
        if ($this->conn->query($sql) !== TRUE) {
            die("Error creating database: " . $this->conn->error);
        }
    }

    // create table if not exist
    private function create_table()
    {
        $sql = "CREATE TABLE IF NOT EXISTS $this->table_name (
            id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, 
            dns_type VARCHAR(10) NOT NULL,
            domain  VARCHAR(30) NOT NULL,
            content VARCHAR(30) NOT NULL,
            ttl INT(6) DEFAULT 120
            )";
        if ($this->conn->query($sql) !== TRUE) {
            die("Error creating table: " . $this->conn->error);
        }
    }

    // insert DNS record
    public function insert_record($type, $domain, $content, $ttl = 120)
    {
        $sql = "INSERT INTO $this->table_name (`dns_type`, `domain`, `content`, `ttl`)
            VALUES ('$type', '$domain', '$content', '$ttl')";
        return $this->conn->query($sql);
    }

    // delete DNS record
    public function delete_record($id)
    {
        $sql = "DELETE FROM $this->table_name WHERE id='$id'";
        return $this->conn->query($sql);
    }

    // return all DNS records
    public function show_records()
    {
        $sql = "SELECT * FROM $this->table_name";
        return $this->conn->query($sql);
    }
}
