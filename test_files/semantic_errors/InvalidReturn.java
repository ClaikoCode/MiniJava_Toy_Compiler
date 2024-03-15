public class InvalidReturn {
    public static void main(String[] a) {
        System.out.println(new MyClass().xyFunc());
    }
}

class classtest {
    int a;
}

class MyClass {
    int x;
    boolean y;
    int[] z;
    MyClass xyz;

    public int xyFunc() {
        return y; // @error - semantic (invalid return type)
    }

    public int xzFunc() {
        return z; // @error - semantic (invalid return type)
    }

    public boolean yxFunc() {
        return x; // @error - semantic ('yxFunc' and its return are of different types)
    }

    public boolean yzFunc() { 
        return this.zxFunc(); // @error - semantic (invalid return type)
    }

    public int[] zxFunc() { 
        return x; // @error - semantic (invalid return type)
    }

    public int[] zyFunc() { 
        return this.yxFunc(); // @error - semantic (invalid return type)
    }

    public int swFunc() { 
        return z[this.yzFunc()]; // @error - semantic (invalid return type)
    }

}