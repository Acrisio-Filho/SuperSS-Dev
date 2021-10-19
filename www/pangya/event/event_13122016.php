<?php
	include_once("source/grand_zodiac_event_system.inc");

	class GrandZodiacEvent extends GrandZodiacEventSystem {

		public function show() {

			$this->checkFirstLogin();

			// Carrega os itens(Raro) que ele ganhou
			$this->loadWinRareList();

			$this->checkGetItem();

            $this->begin();

            echo '<title>Grand Zodiac Event</title>';

            $this->middle();

            // Content

            $this->end();
        }
    }

    // Grand Zodiac Event
    $grand_zodiac_event = new GrandZodiacEvent();

    $grand_zodiac_event->show();
?>