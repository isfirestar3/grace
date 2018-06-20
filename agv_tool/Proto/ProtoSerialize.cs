using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using ProtoDefine;

namespace Tool.Proto
{
    public class Unpackage<PROTO_TYPE> where PROTO_TYPE : ProtoInterface_t, new()
    {
        byte[] byte_stream_ = null;
        PROTO_TYPE ob_ = new PROTO_TYPE();

        public Unpackage(byte[] byteStream)
        {
            byte_stream_ = byteStream;
        }

        public bool build()
        {
            Int32 offset = 0;
            return ob_.build(byte_stream_, ref offset);
        }

        public PROTO_TYPE ob { get { return ob_; } }
    };

    public interface ProtoInterface_t
    {
        int length();
        unsafe bool serialize(byte[] byteStream, ref Int32 offset);
        unsafe bool build(byte[] byteStream, ref Int32 offset);
    }

    public class ProtoCRT_t<T> : ProtoInterface_t where T : struct
    {

        public T value_;

        public ProtoCRT_t()
        {
        }
        public ProtoCRT_t(T _rhs) { value_ = _rhs; }
        public static int type_length() { return Marshal.SizeOf(typeof(T)); }
        public int length() { return Marshal.SizeOf(typeof(T)); }
        public unsafe bool serialize(byte[] byteStream, ref Int32 offset)
        {
            if (null == byteStream || offset + length() > byteStream.Length)
            {
                return false;
            }
            fixed (byte* lp_byte = byteStream)
            {
                Marshal.StructureToPtr(value_, (IntPtr)lp_byte + offset, true);
            }
            offset += length();
            return true;
        }
        public unsafe bool build(byte[] byteStream, ref Int32 offset)
        {
            if (null == byteStream || offset + length() > byteStream.Length)
            {
                return false;
            }
            fixed (byte* lp_byte = byteStream)
            {
                value_ = (T)Marshal.PtrToStructure((IntPtr)lp_byte + offset, typeof(T));
            }
            offset += length();
            return true;
        }
    };

    public class ProtoList_t<T> : List<T>, ProtoInterface_t where T : ProtoInterface_t, new()
    {
        public ProtoList_t() : base() { }
        public int length()
        {
            int sum = 0;
            sum += ProtoCRT_t<UInt32>.type_length();
            foreach (var item in this)
            {
                sum += item.length();
            }
            return sum;
        }
        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            ProtoCRT_t<UInt32> element_count = new ProtoCRT_t<UInt32>((UInt32)base.Count);
            if (!element_count.serialize(byteStream, ref offset))
            {
                return false;
            }
            foreach (var item in this)
            {
                if (!item.serialize(byteStream, ref offset))
                {
                    return false;
                }
            }
            return true;
        }
        public bool build(byte[] _byte_stream, ref Int32 _offset)
        {
            ProtoCRT_t<UInt32> element_count = new ProtoCRT_t<UInt32>();
            if (!element_count.build(_byte_stream, ref _offset))
            {
                return false;
            }
            for (UInt32 i = 0; i < element_count.value_; ++i)
            {
                T item = new T();
                if (!item.build(_byte_stream, ref _offset))
                {
                    return false;
                }
                else
                {
                    base.Add(item);
                }
            }
            return true;
        }
    };

    public class ProtoVector_t<T> : ProtoInterface_t where T : struct
    {

        public T[] value_;

        public ProtoVector_t() { }

        public List<T> to_list()
        {
            List<T> ret_list = new List<T>();
            if (null != value_)
            {
                ret_list.AddRange(value_);
            }
            return ret_list;
        }
        public void from_list(List<T> list, int index, int count)
        {
            if (null == list || 0 > index || 0 >= count)
            {
                value_ = null;
                return;
            }
            else
            {
                value_ = new T[count];
                list.CopyTo(index, value_, 0, count);
            }
        }

        public int length()
        {
            int sum = 0;
            sum += ProtoCRT_t<UInt32>.type_length();
            if (null == value_ || 0 == value_.Length)
            {
                return sum;
            }
            sum += ProtoCRT_t<T>.type_length() * value_.Length;
            return sum;
        }
        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            ProtoCRT_t<UInt32> element_count = new ProtoCRT_t<UInt32>();
            if (null == value_ || 0 == value_.Length)
            {
                element_count.value_ = 0;
            }
            else
            {
                element_count.value_ = (UInt32)value_.Length;
            }
            if (!element_count.serialize(byteStream, ref offset))
            {
                return false;
            }
            if (null == value_ || 0 == value_.Length)
            {
                return true;
            }
            ProtoCRT_t<T> tmp = new ProtoCRT_t<T>();
            foreach (var item in value_)
            {
                tmp.value_ = item;
                if (!tmp.serialize(byteStream, ref offset))
                {
                    return false;
                }
            }
            return true;
        }
        public bool build(byte[] byteStream, ref Int32 offset)
        {
            ProtoCRT_t<UInt32> element_count = new ProtoCRT_t<UInt32>();
            if (!element_count.build(byteStream, ref offset))
            {
                return false;
            }
            if (0 == element_count.value_)
            {
                value_ = null;
                return true;
            }

            value_ = new T[element_count.value_];
            ProtoCRT_t<T> tmp = new ProtoCRT_t<T>();
            for (UInt32 i = 0; i < element_count.value_; ++i)
            {
                if (!tmp.build(byteStream, ref offset))
                {
                    return false;
                }
                value_[i] = tmp.value_;
            }
            return true;
        }
    };

    public class ProtoStringGB2312_t : ProtoInterface_t
    {

        public string value_;

        public ProtoStringGB2312_t() : base() { }
        public ProtoStringGB2312_t(string value) : base()
        {
            value_ = value;
        }
        public int length()
        {
            int sum = 0;
            sum += ProtoCRT_t<UInt32>.type_length();
            if (null != value_ && value_.Length != 0)
            {
                byte[] r_byte = StringHelper.String2GB2312(value_);
                sum += r_byte.Length;
            }
            return sum;
        }
        public bool serialize(byte[] byteStream, ref Int32 offset)
        {
            byte[] r_byte = null;
            ProtoCRT_t<UInt32> element_count = new ProtoCRT_t<UInt32>(0);
            if (null == value_ || 0 == value_.Length)
            {
                element_count.value_ = 0;
            }
            else
            {
                r_byte = StringHelper.String2GB2312(value_);
                element_count.value_ = (UInt32)r_byte.Length;
            }
            if (!element_count.serialize(byteStream, ref offset))
            {
                return false;
            }
            if (null == r_byte)
            {
                return true;
            }
            if (offset + r_byte.Length > byteStream.Length)
            {
                return false;
            }

            Buffer.BlockCopy(r_byte, 0, byteStream, offset, r_byte.Length);
            offset += r_byte.Length;
            return true;
        }
        public bool build(byte[] byteStream, ref Int32 offset)
        {
            ProtoCRT_t<UInt32> element_count = new ProtoCRT_t<UInt32>();
            if (!element_count.build(byteStream, ref offset))
            {
                return false;
            }
            if (0 == element_count.value_)
            {
                value_ = "";
                return true;
            }
            else if (offset + element_count.value_ > byteStream.Length)
            {
                return false;
            }

            byte[] b_data = new byte[element_count.value_];
            Buffer.BlockCopy(byteStream, offset, b_data, 0, (int)element_count.value_);
            offset += (int)element_count.value_;
            value_ = StringHelper.GB2312ToString(b_data);
            return true;
        }
    };
}
