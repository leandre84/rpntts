namespace Rpntts\Model;

 class Booking
 {
     public $pk;
     public $timeStamp;
     public $type;

     public function exchangeArray($data)
     {
         $this->pk     = (!empty($data['pk'])) ? $data['pk'] : null;
         $this->timeStamp = (!empty($data['timeStamp'])) ? $data['timeStamp'] : null;
         $this->type  = (!empty($data['type'])) ? $data['type'] : null;
     }
 }