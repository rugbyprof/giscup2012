<?php
/*========================================================
/* Route distance class.
/* 
/* Terry Griffin
/* terry.griffin@mwsu.edu
/* 
/* This software is provided as-is, with no warranty.
/* Please use and modify freely for anything you like :)
/* Version 1.0 - 20 Sep 2010
/*
/* Based on douglass-peucker code written by:
/* 		Anthony Cartmell
/* 		ajcartmell@fonant.com
/*========================================================*/

class Route
{
    private $A = array();
	private $current;

    public function __construct($geopoints_array)
    {
        foreach ($geopoints_array as $point)
        {
            $this->A[] = new Vector($point->latitude,$point->longitude);
        }
		$this->current = 0;
    }

    /**
	 * Returns the total number of points in $this->A
     *
     * @param none
	 * @returns int 
     */	
	public function RouteSize(){
		return sizeof($this->A);
	}

    /**
	 * Returns a point indexed by $index in $this->A
     *
     * @param int index
	 * @returns vector 
     */	
	public function GetPoint($index){
		return $this->A[$index];
	}

    /**
	 * Get distance from $this->A to Route $B
	 * Calculates the avg distance from every point in $this->A to the closest line segment
	 * in $B
     *
     * @param Route $B
	 * @returns int 
     */
    public function DistanceFrom($B)
    {
		$SumDistance=0.0;

		//loop through $this->A comparing each point to a line segment on $B
		for ($i=0;$i<sizeof($this->A);$i++){
		
			//Set min distance to some arbitrarily large number
			$min_dist_to_line = pow(2,20);
			
			for ($j=0;$j<$B->RouteSize()-1;$j++){
			    $line = new Line($B->GetPoint($j),$B->GetPoint($j+1));
				$dist_to_line = $line->DistanceToPoint($this->A[$i]);
				if ($dist_to_line<$min_dist_to_line)
				{
					$min_dist_to_line = $dist_to_line;
				}
			}
			$SumDistance += $min_dist_to_line;
		}
		return $SumDistance/sizeof($this->A);
    }    
}


class Vector
{
    public $x;
    public $y;
    public $include;

    public function __construct($x,$y)
    {
        $this->x = $x;
        $this->y = $y;
    }

    public function DotProduct(Vector $v)
    {
        $dot = ($this->x * $v->x + $this->y * $v->y);
        return $dot;
    }

    public function Magnitude()
    {
        return sqrt($this->x*$this->x + $this->y*$this->y);
    }

    public function UnitVector()
    {
        if ($this->Magnitude()==0) return new Vector(0,0);
        return new Vector($this->x/$this->Magnitude(),$this->y/$this->Magnitude());
    }
}

class Line 
{
    public $p1;
    public $p2;

    public function __construct(Vector $p1,Vector $p2)
    {
        $this->p1 = $p1;
        $this->p2 = $p2;
    }

    public function LengthSquared()
    {
        $dx = $this->p1->x - $this->p2->x;
        $dy = $this->p1->y - $this->p2->y;
        return $dx*$dx + $dy*$dy;
    }
	
    public function DistanceToPoint(Vector $point)
    {
		$distance = 0;
        $v = new Vector($point->x - $this->p1->x, $point->y - $this->p1->y);
        $l = new Vector($this->p2->x - $this->p1->x, $this->p2->y - $this->p1->y);
        $dot = $v->DotProduct($l->UnitVector());
        if ($dot<=0) // Point nearest P1
        {
            $dl = new Line($this->p1,$point);
            $distance = sqrt($dl->LengthSquared());
        }
        if (($dot*$dot)>=$this->LengthSquared())  // Point nearest P2
        {
            $dl = new Line($this->p2,$point);
            $distance = sqrt($dl->LengthSquared());
        }
        else // Point within line
        {
            $v2 = new Line($this->p1,$point);
            $h = sqrt($v2->LengthSquared());
            $distance = ($h - $dot*$dot);
        }
		
		return sqrt($distance);
    }
	
}

class GeoPoint {
    public $latitude;
    public $longitude;

    public function __construct($lat,$lng)
    {
        $this->latitude = (float)$lat;
        $this->longitude = (float)$lng;
    }
};


