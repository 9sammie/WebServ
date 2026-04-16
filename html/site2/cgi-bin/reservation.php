#!/usr/bin/php
<?php

$raw_data = file_get_contents("php://stdin");


parse_str($raw_data, $post_vars);

$name = isset($post_vars['client_name']) ? htmlspecialchars($post_vars['client_name']) : "Client mystère";
$guests = isset($post_vars['guests']) ? (int)$post_vars['guests'] : 0;
$time = isset($post_vars['res_time']) ? $post_vars['res_time'] : "non précisée";


$time_valid = true;
$error_message = "";

if ($time !== "non précisée") {
    $time_parts = explode(":", $time);
    if (count($time_parts) === 2) {
        $hour = (int)$time_parts[0];
        $minute = (int)$time_parts[1];
        
        if ($hour < 11 || $hour >= 23) {
            $time_valid = false;
            $error_message = "Nous sommes ouverts de 11h à 22h59. Veuillez choisir un horaire valide.";
        }
    }
}

if ($time_valid) {
    $reservation = array(
        'timestamp' => date('Y-m-d H:i:s'),
        'client_name' => $name,
        'guests' => $guests,
        'res_time' => $time
    );

    $reservations_file = dirname(__FILE__) . '/reservations.json';
    $reservations = array();

    if (file_exists($reservations_file)) {
        $json_data = file_get_contents($reservations_file);
        if ($json_data) {
            $reservations = json_decode($json_data, true);
            if (!is_array($reservations)) {
                $reservations = array();
            }
        }
    }

    $reservations[] = $reservation;

    file_put_contents($reservations_file, json_encode($reservations, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE));
}

echo "Content-type: text/html\r\n\r\n";

?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: 'Georgia', serif; text-align: center; padding: 50px; background: #fffcf5; }
        .confirmation { border: 2px dashed #d4af37; padding: 20px; display: inline-block; }
        .chef { color: #e63946; font-style: italic; }
        a {
            display: inline-block;
            margin-top: 20px;
            padding: 10px 20px;
            background-color: #d4af37;
            color: white;
            text-decoration: none;
            border-radius: 5px;
            font-weight: bold;
        }
        a:hover {
            background-color: #b8941f;
        }
    </style>
</head>
<body>
    <div class="confirmation">
        <?php if ($time_valid): ?>
            <h1>Confirmation de Réservation</h1>
            <p>Merci, <strong><?php echo $name; ?></strong> !</p>
            
            <?php if ($guests > 8): ?>
                <p class="chef">"Ah ! Une grande tablée de <?php echo $guests; ?> personnes ? Nous allons préparer la salle VIP."</p>
            <?php else: ?>
                <p>Votre table pour <strong><?php echo $guests; ?></strong> personnes est réservée pour <strong><?php echo $time; ?></strong>.</p>
            <?php endif; ?>
            
            <p><em>Tout le monde peut cuisiner, mais seuls les plus intrépides réussissent !</em></p>
            <br><br>
            <a href="#" onclick="window.location.href = 'http://' + window.location.hostname + ':8080/'; return false;">Retour au Site 1</a>
        <?php else: ?>
            <h1 style="color: #e63946;">Erreur de Réservation</h1>
            <p style="color: #e63946; font-weight: bold;"><?php echo $error_message; ?></p>
            <p>Nos horaires d'ouverture : <strong>11h à 22h59</strong></p>
            <br><br>
            <a href="/reservation.html">Retour au formulaire</a>
        <?php endif; ?>
    </div>
</body>
</html>