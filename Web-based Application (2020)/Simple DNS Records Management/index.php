<!DOCTYPE html>
<html>

<head>
    <title>Simple DNS Records Management</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="css/uikit.min.css" />
    <script src="js/uikit.min.js"></script>
    <script src="js/uikit-icons.min.js"></script>
</head>

<body>
    <?php
    require_once('config.php');
    require_once('database.php');

    $db = new DNS_DB($CONFIG);

    // show alert messages
    function notify($msg, $level = 1)
    {
        switch ($level) {
            case 1:
                echo '<div class="uk-alert-primary" uk-alert>';
                break;
            case 2:
                echo '<div class="uk-alert-success" uk-alert>';
                break;
            case 3:
                echo '<div class="uk-alert-warning" uk-alert>';
                break;
            case 4:
                echo '<div class="uk-alert-danger" uk-alert>';
                break;
        }
        echo '<a class="uk-alert-close" uk-close></a>';
        echo "<p>$msg</p>";
        echo '</div>';
        return $msg;
    }

    function insert($db)
    {
        $flag = "";

        // check values
        if (isset($_GET['type']) && !empty($_GET['type'])) {
            $type = $_GET['type'];
        } else {
            $flag = notify("[Type] cannot be empty!", 3);
        }
        if (isset($_GET['domain']) && !empty($_GET['domain'])) {
            $domain = $_GET['domain'];
        } else {
            $flag = notify("[Domain] cannot be empty!", 3);
        }
        if (isset($_GET['content']) && !empty($_GET['content'])) {
            $content = $_GET['content'];
        } else {
            $flag = notify("[Content] cannot be empty!", 3);
        }
        if (isset($_GET['ttl']) && !empty($_GET['ttl'])) {
            $ttl = $_GET['ttl'];
            is_numeric($ttl) && intval($ttl) >= 0 ?: $flag = notify("TTL must >= 0", 3);
        } else {
            $flag = notify("[TTL] cannot be empty!", 3);
        }

        if (strlen($flag) == 0)
            $db->insert_record($type, $domain, $content, $ttl);
        else
            notify("Insert record failed", 4);
    }

    function delete($db)
    {
        if (isset($_GET['id'])) {
            $db->delete_record($_GET['id']);
        } else {
            notify("Delete item's ID is missing", 3);
        }
    }

    switch ($_GET['action']) {
        case 'insert':
            insert($db);
            break;
        case 'delete':
            delete($db);
            break;
        default:
            empty($_GET['action']) ?: notify("There is no action: '" . $_GET['action'] . "'", 3);
    }
    ?>
    <br><br>
    <div class="uk-container uk-container-small">
        <h1 class="uk-heading-medium uk-text-center">Simple DNS Records Management</h1>
    </div>
    <br><br>
    <div class="uk-container uk-container-small">
        <form class="uk-grid-small" uk-grid action="index.php" method="get">
            <div class="uk-width-1-4@s">
                <label class="uk-form-label">TYPE</label>
                <select class="uk-select" name="type">
                    <option value="A">A</option>
                    <option value="AAAA">AAAA</option>
                    <option value="CNAME">CNAME</option>
                    <option value="TXT">TXT</option>
                </select>
            </div>
            <div class="uk-width-1-4@s">
                <label class="uk-form-label">DOMAIN</label>
                <input class="uk-input" type="text" name="domain">
            </div>
            <div class="uk-width-1-4@s">
                <label class="uk-form-label">CONTENT</label>
                <input class="uk-input" type="text" name="content">
            </div>
            <div class="uk-width-1-4@s">
                <label class="uk-form-label">TTL</label>
                <input class="uk-input" type="number" name="ttl" value="120" placeholder="120">
            </div>
            <div class="uk-width-1-1@s">
                <button class="uk-button uk-button-primary uk-width-1-1" name="action" value="insert">ADD</button>
            </div>
        </form>
        <br>
    </div>
    <br><br>
    <div class="uk-container uk-container-small">

        <table class="uk-table uk-table-small uk-table-divider uk-table-hover">
            <thead>
                <tr>
                    <!-- <th>ID</th> -->
                    <th>Type</th>
                    <th>Domain</th>
                    <th>Content</th>
                    <th>TTL</th>
                    <th>Delete</th>
                </tr>
            </thead>
            <tbody>
                <?php
                function print_data($row)
                {
                    echo "<tr>";
                    // echo "<td>" . $row['id'] . "</td>";
                    echo "<td>" . $row['dns_type'] . "</td>";
                    echo "<td>" . $row['domain'] . "</td>";
                    echo "<td>" . $row['content'] . "</td>";
                    echo "<td>" . $row['ttl'] . "</td>";
                    echo "<td><a href=\"index.php?id=" . $row['id'] . "&action=delete\" class=\"uk-icon-link\" uk-icon=\"trash\"></a></td>";
                    echo "</tr>";
                }

                $result = $db->show_records();
                // print all records
                while ($row = $result->fetch_array()) {
                    print_data($row);
                }
                ?>
            </tbody>
        </table>
    </div>
</body>
</html>