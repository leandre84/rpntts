<?php
namespace Rpntts\Model;

use Zend\Db\TableGateway\TableGateway;

class TimeModelTable
{
    protected $tableGateway;
    
    public function __construct(TableGateway $tableGateway)
    {
        $this->tableGateway = $tableGateway;
    }

    public function fetchAll()
    {
        $resultSet = $this->tableGateway->select();
        
        return $resultSet;
    }

    public function getTimeModel($id)
    {
        $id  = (int) $id;
        $rowset = $this->tableGateway->select(array('pk' => $id));
        $row = $rowset->current();
        if (!$row) {
            throw new \Exception("Could not find row "  . $id);
        }
        
        return $row;
    }

    public function saveTimeModel(TimeModel $timeModel)
    {
        $data = array(
            'pk' => $timeModel->primaryKey,
            'modelname' => $timeModel->modelName,
            'monday' => $timeModel->monday,
            'tuesday' => $timeModel->tuesday,
            'wednesday' => $timeModel->wednesday,
            'thursday' => $timeModel->thursday,
            'friday' => $timeModel->friday,
            'saturday' => $timeModel->saturday,
            'sunday' => $timeModel->sunday,
        );

        $id = (int) $timeModel->primaryKey;
        if ($id == 0) {
            $this->tableGateway->insert($data);
        } else {
            if ($this->getTimeModel($id)) {
                $this->tableGateway->update($data, array('pk' => $id));
            } else {
                throw new \Exception("Time model id " . $id . " does not exist");
            }
        }
    }

    public function deleteTimeModel($id)
    {
        $this->tableGateway->delete(array('pk' => (int) $id));
    }
 }
 