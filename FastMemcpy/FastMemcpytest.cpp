#include "checkpoint.h"
#include "FastMemcpy.h"
#include <iostream>
#include <string>
#include <conio.h>
#include <cstdlib>

namespace {
	typedef std::uint32_t TYPE;

#if defined(__INTEL_COMPILER) || defined(__GXX_EXPERIMENTAL_CXX0X__)

#ifdef _WIN64
	static constexpr std::int32_t ELEMSIZE = 200000000;
#else
	static constexpr std::int32_t ELEMSIZE = 100000000;
#endif

#ifdef _OPENMP
	static const std::string str = "OpenMP";
#else
	static const std::string str = "Intel Cilk Plus";
#endif

#else

#ifdef _WIN64
	static const std::int32_t ELEMSIZE = 200000000;
#else
	static const std::int32_t ELEMSIZE = 100000000;
#endif

#ifdef _OPENMP
	static const std::string str("OpenMP");
#else
	static const std::string str("Intel Cilk Plus");
#endif

#endif

}

int main()
{
	std::cout << ((ELEMSIZE * sizeof(TYPE)) >> 20) << " MiB�̃������R�s�[�̃x���`�}�[�N���s���܂�\n";

	try {
		const FastMemcpy::smart_pointer<TYPE>::type aligned_src(FastMemcpy::aligned_allocate_init<TYPE>(ELEMSIZE));
		FastMemcpy::smart_pointer<TYPE>::type aligned_dst(FastMemcpy::aligned_allocate_init<TYPE>(ELEMSIZE));
	
		const FastMemcpy::smart_pointer<TYPE>::type src(FastMemcpy::allocate_init<TYPE>(ELEMSIZE));
		FastMemcpy::smart_pointer<TYPE>::type dst(FastMemcpy::allocate_init<TYPE>(ELEMSIZE));
	
        checkpoint::CheckPoint cp;

        cp.checkpoint("�����J�n", __LINE__);

		{
			FastMemcpy::copy<TYPE>(aligned_src, aligned_dst, ELEMSIZE);
			cp.checkpoint("�W����memcpy�i�A���C�����g�������Ă���ꍇ�j", __LINE__);

#ifdef _DEBUG
			FastMemcpy::memcheck<TYPE>(aligned_src, aligned_dst, ELEMSIZE);
#endif

			FastMemcpy::aligned_copy<TYPE, false>(aligned_src, aligned_dst, ELEMSIZE);
			cp.checkpoint("�����memcpy�i�A���C�����g�������Ă���ꍇ�ASSE2�g�p�j", __LINE__);

#ifdef _DEBUG
			FastMemcpy::memcheck<TYPE>(aligned_src, aligned_dst, ELEMSIZE);
#endif

			FastMemcpy::aligned_copy<TYPE, true>(aligned_src, aligned_dst, ELEMSIZE);
#if defined(__INTEL_COMPILER) || defined(__GXX_EXPERIMENTAL_CXX0X__)
			static const std::string tmpstr("�����memcpy�i�A���C�����g�������Ă���ꍇ�ASSE2+" + str + "�g�p�j");
#else
			const std::string tmpstr("�����memcpy�i�A���C�����g�������Ă���ꍇ�ASSE2+" + str + "�g�p�j");
#endif
			cp.checkpoint(tmpstr.c_str(), __LINE__);

#ifdef _DEBUG
			FastMemcpy::memcheck<TYPE>(aligned_src, aligned_dst, ELEMSIZE);
#endif
		}

		{
			FastMemcpy::copy<TYPE>(src, dst, ELEMSIZE);
			cp.checkpoint("�W����memcpy�i�A���C�����g�������Ă��Ȃ��ꍇ�j", __LINE__);

#ifdef _DEBUG
			FastMemcpy::memcheck<TYPE>(src, dst, ELEMSIZE);
#endif
		
			FastMemcpy::copy<TYPE, false>(src, dst, ELEMSIZE);
			cp.checkpoint("�����memcpy�i�A���C�����g�������Ă��Ȃ��ꍇ�ASSE2�g�p�j", __LINE__);

#ifdef _DEBUG
			FastMemcpy::memcheck<TYPE>(src, dst, ELEMSIZE);
#endif
		
			FastMemcpy::copy<TYPE, true>(src, dst, ELEMSIZE);
#if defined(__INTEL_COMPILER) || defined(__GXX_EXPERIMENTAL_CXX0X__)
			static const std::string tmpstr("�����memcpy�i�A���C�����g�������Ă���ꍇ�ASSE2+" + str + "�g�p�j");
#else
			const std::string tmpstr("�����memcpy�i�A���C�����g�������Ă���ꍇ�ASSE2+" + str + "�g�p�j");
#endif
			cp.checkpoint(tmpstr.c_str(), __LINE__);

#ifdef _DEBUG
			FastMemcpy::memcheck<TYPE>(src, dst, ELEMSIZE);
#endif
		}
	
		chk.checkpoint_print();
	} catch (const FastMemcpy::system_error & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	} catch (const std::runtime_error & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "�I������ɂ͉����L�[�������Ă�������..." ;
	::_getch();

	return EXIT_SUCCESS;
}
