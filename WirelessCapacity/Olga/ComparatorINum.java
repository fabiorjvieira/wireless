import java.util.Comparator;

public class ComparatorINum implements Comparator<Link>{

	public int compare(Link l1, Link l2) {
		
//		Link l1 = (Link) arg0;
//		Link l2 = (Link) arg1;
		if (l1.id == l2.id) return 0;
		if (l1.interferenceNumber > l2.interferenceNumber) return -1;
		if (l1.interferenceNumber < l2.interferenceNumber) return 1;
		if (l1.interferenceNumber == l2.interferenceNumber) return 0;
		
		return 0;
	}	
}
