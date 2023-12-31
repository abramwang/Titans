try:
    from setuptools import setup, find_packages
except ImportError:
    from distutils.core import setup, find_packages

setup(
    name='pymoneta',  #
    author='bo.wang',  #
    version='0.1.0',  #
    license='MIT',

    description='olla',  #
    long_description='''long description''',
    author_email='wang.b@quantstage.com',  #
    url='https://www.quantstage.com',  #
    
    packages= find_packages(exclude=['pymoneta',]),
    # packages=["pyolla"],
    # keywords='NLP,tokenizing,Chinese word segementation',
    package_data={'':['*.*']},

    install_requires=[
        "redis >= 1.0.0",
    ],
    python_requires='>=3',
    classifiers=[
        # 'Development Status :: 4 - Beta',
        # 'Operating System :: Microsoft'  #
        'Intended Audience :: Developers',
        # 'License :: OSI Approved :: MIT License',
        # 'License :: OSI Approved :: BSD License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries'
    ],
    zip_safe = False,
)